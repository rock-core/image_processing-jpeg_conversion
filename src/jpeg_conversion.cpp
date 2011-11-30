#include "jpeg_conversion.hpp"

namespace conversion {

bool JpegConversion::compress(base::samples::frame::Frame& frame_input) {
    if(frame_input.getFrameMode() == base::samples::frame::MODE_PJPG) {
        std::cout << "Image already compressed, done" << std::endl;
        return true;
    }

    // Create temporary buffer big enough for the jpeg.
    unsigned int new_buffer_size = frame_input.getWidth() * frame_input.getHeight() * 
            frame_input.getPixelSize();

    // Create our in-memory output buffer to hold the jpeg.
    if(new_buffer_size > mBufferSize) {
        if(mBuffer != NULL) {
            delete mBuffer;
        }
        mBuffer = new unsigned char[new_buffer_size];
        mBufferSize = new_buffer_size;
    }

    jpeg_create_compress(&cinfo);
    cinfo.err = jpeg_std_error(&jerr);

    // Define destination.
    mDestMgr.next_output_byte    = mBuffer;
    mDestMgr.free_in_buffer      = mBufferSize;
    mDestMgr.init_destination    = init_buffer; // method
    mDestMgr.empty_output_buffer = empty_buffer; // method
    mDestMgr.term_destination    = term_buffer; // method
    cinfo.dest = &mDestMgr;

    cinfo.image_width      = frame_input.getWidth();
    cinfo.image_height     = frame_input.getHeight();
    cinfo.input_components = frame_input.getPixelSize();
    cinfo.in_color_space   = JCS_RGB;
 
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality (&cinfo, JPEG_QUALITY, true);
    jpeg_start_compress(&cinfo, true);
 
    JSAMPROW row_pointer;
 
    uint8_t* image = frame_input.getImagePtr();
    int row_size = frame_input.getRowSize();
    // Silly bit of code to get the RGB in the correct order .
    // Not needed as it seems.
    /*
    int width = frame_input.getWidth();
    int height = frame_input.getHeight();
    int pixel_size = frame_input.getPixelSize();
    for (int x = 0; x < width; x++) {
	    for (int y = 0; y < height; y++) {
		    uint8_t *p    = (uint8_t *) image + 
                    y * row_size + x * pixel_size;
		    std::swap (p[0], p[2]);
	    }
    }
    */
    // Main code to write jpeg data.
    while (cinfo.next_scanline < cinfo.image_height) { 		
	    row_pointer = (JSAMPROW) &image[cinfo.next_scanline * row_size];
	    jpeg_write_scanlines(&cinfo, &row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);

    // Calculate size of the jpeg.
    int jpeg_size = cinfo.dest->next_output_byte - mBuffer;

    // Initialize member frame to PJPG with calculated size.
    mFrameOutput.init(frame_input.getWidth(), frame_input.getHeight(), 24, 
            base::samples::frame::MODE_PJPG, 0, jpeg_size);

   
    // Copy jpeg to Frame buffer.
    mFrameOutput.setImage((const char*)mBuffer, jpeg_size);

    jpeg_destroy_compress(&cinfo);

    return true;
}

bool JpegConversion::decompress(base::samples::frame::Frame& frame_input) {  
    if(frame_input.getFrameMode() == base::samples::frame::MODE_RGB) {
        std::cout << "Image already decompressed, done" << std::endl;
        return true;
    }

    // Initialize member frame to RGB 24.
    mFrameOutput.init(frame_input.getWidth(), frame_input.getHeight(), 8, base::samples::frame::MODE_RGB);

    jpeg_create_decompress(&dinfo);
    dinfo.err = jpeg_std_error(&jerr);

    // Define source (input frame).
    mSrcMgr.next_input_byte = frame_input.getImagePtr();
    mSrcMgr.bytes_in_buffer = frame_input.getNumberOfBytes();
    // Set source methods.
    mSrcMgr.init_source = my_init_source;
    mSrcMgr.fill_input_buffer = my_fill_input_buffer;
    mSrcMgr.skip_input_data = my_skip_input_data;
    mSrcMgr.resync_to_restart = my_resync_to_restart;
    mSrcMgr.term_source = my_term_source;
    dinfo.src = &mSrcMgr;

    // Write output file header.
    // Size should already be set.
    dinfo.image_width =  frame_input.getWidth() ; 
    dinfo.image_height = frame_input.getHeight(); 
    dinfo.out_color_components = COLOR_COMPONENTS;       
    dinfo.out_color_space = JCS_RGB;

    // Read file header, set default decompression parameters.
    (void) jpeg_read_header(&dinfo, TRUE);

    // Start decompressor.
    (void) jpeg_start_decompress(&dinfo);


    int row_stride =  dinfo.output_width * dinfo.output_components; // Physical row width in output buffer.

    // Decompress jpeg row by row.
    unsigned char* pos = mFrameOutput.getImagePtr(); // Write directly into new RGB frame.
    unsigned char* pos_end = mFrameOutput.getLastByte() + 1; // Use first byte after image memory.
    while (dinfo.output_scanline < dinfo.output_height) {
        // jpeg_read_scanlines expects an array of pointers to scanlines.
        // Here the array is only one element long, but you could ask for
        // more than one scanline at a time if that's more convenient.
        if(pos + row_stride <= pos_end) {
            (void) jpeg_read_scanlines(&dinfo, &pos, 1);
            //memcpy(pos, sample_array, row_stride);
            pos += row_stride;
        } else {
            std::cerr << "RGB buffer full, stop decompressing" << std::endl;
            return false;
        }
    }

    // Finish decompression and release memory.
    //(*dest_mgr->finish_output) (&dinfo, dest_mgr);
    (void) jpeg_finish_decompress(&dinfo);
    jpeg_destroy_decompress(&dinfo);

    return true;
}

bool JpegConversion::storeFrame(std::string filename) {
    if(mFrameOutput.getNumberOfBytes() == 0) {
        std::cerr << "Frame is empty." << std::endl;
        return false;
    }

    if(mFrameOutput.getFrameMode() == base::samples::frame::MODE_RGB) {
        filename += ".ppm";
    }
    

    FILE* pFile  = fopen(filename.c_str(), "wb");
    if(pFile == NULL) {
        std::cerr << "File " << filename << "could not be opened." << std::endl;
        return false;
    }

    // Store as ppm
    if(mFrameOutput.getFrameMode() == base::samples::frame::MODE_RGB) {
        char buffer[32];
        int written = 0;
        int width = mFrameOutput.getWidth();
        int height = mFrameOutput.getHeight();
        // Write header.
        written = snprintf(buffer, 32, "%c%c%c%d %d\n%d\n", 'P', '3', '\n', width, height, 255);
        fwrite(buffer, 1, written, pFile);
        
        unsigned char* pos = mFrameOutput.getImagePtr();
        unsigned char* end = mFrameOutput.getLastByte();
        int counter = 0;
        for(;pos <= end; pos++) {
            fprintf(pFile, "%d ", (int)*pos);
            counter++;
        }
        std::cout << "Bytes written: " << counter << " Pixels: " << counter / 3.0 << std::endl;
    } else {
        // Store as jpeg, just write complete data.
        unsigned int bytes_written = fwrite(mFrameOutput.getImagePtr(), 1, 
                mFrameOutput.getNumberOfBytes(), pFile);
        if(bytes_written != mFrameOutput.getNumberOfBytes()) {
            std::cerr << "Only " << bytes_written << "/" << mFrameOutput.getNumberOfBytes() << 
                    " could be written." << std::endl;
            return false;
        }
    }
    fclose(pFile);
    return true;   
}

bool JpegConversion::loadJpeg(std::string filename, uint32_t width, uint32_t height) {
    FILE* pFile  = fopen(filename.c_str(), "rb");
    if(pFile == NULL) {
        std::cerr << "File " << filename << " could not be opened." << std::endl;
        return false;
    }

    // obtain file size:
    fseek (pFile , 0 , SEEK_END);
    long jpeg_size = ftell (pFile);
    rewind (pFile);

    mFrameOutput.init(width, height, 24, base::samples::frame::MODE_PJPG, 0, jpeg_size);
    
    // copy the file into the buffer:
    long result = fread (mFrameOutput.getImagePtr(), 1, jpeg_size, pFile);
    if (result != jpeg_size) {
        std::cerr << "Only " << result << "/" << jpeg_size << "could be read." << std::endl;
        return false;
    }

    return true;
}

} // end namespace conversion
