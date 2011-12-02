#ifndef _JPEG_CONVERSION_H_
#define _JPEG_CONVERSION_H_

#include <iostream>
#include <exception>
#include <stdexcept>
#include <string>

#include "jpeglib.h"

#include "base/samples/frame.h"

namespace conversion {

/**
 * Only 8bit channels are used.
 */
class JpegConversion {

 public: // STATICs
    const static int JPEG_QUALITY = 80;

 public:
    JpegConversion() : mBuffer(NULL), mBufferSize(0), mFlipBuffer(NULL), mFlipBufferSize(0),
            cinfo(), dinfo(), jerr(), mSrcMgr(), mDestMgr(), mJpegQuality(JPEG_QUALITY) { 
    }

    JpegConversion(unsigned int jpeg_quality) : mBuffer(NULL), mBufferSize(0),
            cinfo(), dinfo(), jerr(), mSrcMgr(), mDestMgr(), mJpegQuality(jpeg_quality) {
        if(mJpegQuality > 100)
            mJpegQuality = 100;
    }

    ~JpegConversion() {
        if(mBuffer != NULL) {
            delete mBuffer;
            mBuffer = NULL;
        }
    }

    /**
     * Compresses the passed image to JPEG. Supported frame color formats are MODE_GRAYSCALE,
     * MODE_RGB, MODE_BGR, MODE_JPEG and MODE_UYVY. To keep 'frame_input' constant, using MODE_BGR
     * creates an extra copy of the passed image. Using the jpeg quality passed in the constructor.
     * Throws a std::runtime_error if the color mode of the input image is not supported.
     * \param frame_input Image which should be converted to jpeg-format. Input frame has to 
     * have one of the valid color modes.
     * \param frame_output Will be set to MODE_JPEG and resized if required.
     */
    void compress(base::samples::frame::Frame const& frame_input, base::samples::frame::Frame& frame_output);

    /**
     * Decompress the passed frame to the color format of 'frame_output'.
     * Throws a std::runtime_error if the input image is not a jpeg or the 
     * color format of the output image is not supported.
     * \param frame_input Input image, must be of color format MODE_JPEG.
     * \param frame_output Has to contain the color format (MODE_GRAYSCALE,
     * MODE_RGB, MODE_BGR, MODE_JPEG and MODE_UYVY) the input jpeg should be converted to.
     * Will be resized if necessary. Color channel data depth will be set to 8.
     */
    void decompress(base::samples::frame::Frame const& frame_input, base::samples::frame::Frame& frame_output);

    /**
     * Maps the frame colorspace to the IJG colorspace.
     * \param flip_rgb Is set to true if the frame colorspace is BGR (IJG just handles RGB). 
     * The R and B channel have to be switched manually before compression and accordingly 
     * after decompression.
     * \param is_jpeg Is set to true if the passed frame has already a jpeg format.
     */
    J_COLOR_SPACE getJpegColorspace(base::samples::frame::frame_mode_t const frame_mode, 
            bool& flip_rgb, bool& is_jpeg);

    /**
     * If available, the Frame will be stored to 'filename'.
     * If a folder is not available, the file will not be created.
     * \return True on success.
     */
    bool storeFrame(std::string filename, base::samples::frame::Frame const& frame);

    /**
     * Loads the jpeg and store its content to 'mFrameOutput'.
     */
    bool loadJpeg(std::string const& filename, uint32_t const width, uint32_t const height,
            base::samples::frame::Frame& frame);

 private:
    /**
     * Copies color channels from input to output flipping R and B.
     * \param img_size Size of input and output image.
     * \param input_img Original image copied to output with flipped R and B channel.
     * \param output_img Receive the flipped color channels.
     */
    void switchRB(uint32_t const img_size, uint8_t const* input_img, 
            uint8_t* output_img);

    /**
     * Switch the color channels R and B of the input image. 
     */
    void switchRB(uint32_t const img_size, uint8_t* input_img);

 private: // REQUIRED jpeg methods.
    // COMPRESS
    /**
     * Setup the buffer but we did that in the main function.
     */
    static void init_buffer(jpeg_compress_struct* cinfo) {}
     
    /** 
     * What to do when the buffer is full; this should almost never
     * happen since we allocated our buffer to be big to start with
     */
    static boolean empty_buffer(jpeg_compress_struct* cinfo) {
	    return TRUE;
    }

    /** 
     * Finalize the buffer and do any cleanup stuff. 
     */
    static void term_buffer(jpeg_compress_struct* cinfo) {}

    // DECOMPRESS
    static void my_init_source(jpeg_decompress_struct* cinfo) {}
    static boolean my_fill_input_buffer(jpeg_decompress_struct* cinfo) {return true;}
    static void my_skip_input_data(jpeg_decompress_struct* cinfo, long num_bytes) {}
    static boolean my_resync_to_restart(jpeg_decompress_struct* cinfo, int desired) {return true;}
    static void my_term_source(jpeg_decompress_struct* cinfo) {}

 private:
    unsigned char* mBuffer;
    unsigned int mBufferSize;

    unsigned char* mFlipBuffer; // Used to convert BGR to RGB.
    unsigned int mFlipBufferSize;

    struct jpeg_compress_struct cinfo;
	struct jpeg_decompress_struct dinfo; 
    struct jpeg_error_mgr jerr;
    struct jpeg_source_mgr mSrcMgr;
    struct jpeg_destination_mgr mDestMgr;

    int mJpegQuality;
};
} // end namespace conversion
#endif
