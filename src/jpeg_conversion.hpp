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
 * Converts PJPG to RGB 24 (3*8) base::samples::frame::Frame(s) and vice versa.
 */
class JpegConversion {

 public: // STATICs
    const static int JPEG_QUALITY = 80;
    const static int COLOR_COMPONENTS = 3;

 public:
    JpegConversion() : mFrameOutput(), mBuffer(NULL), mBufferSize(0),
            cinfo(), dinfo(), jerr(), mSrcMgr(), mDestMgr(), mJpegQuality(JPEG_QUALITY)
    {
        
    }

    JpegConversion(unsigned int jpeg_quality) : mFrameOutput(), mBuffer(NULL), mBufferSize(0),
            cinfo(), dinfo(), jerr(), mSrcMgr(), mDestMgr(), mJpegQuality(jpeg_quality)
    {
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
     * Decompresses the passed RGB 24 to PJPEG.
     */
    bool compress(base::samples::frame::Frame& frame_input);

    /**
     * Decompresses the passed PJPEG to RGB 24.
     * Writes directly into the mFrameOutput buffer.
     */
    bool decompress(base::samples::frame::Frame& frame_input);

    inline base::samples::frame::Frame getFrame() {
        return mFrameOutput;
    }

    /**
     * If available, the Frame will be stored to 'filename'.
     * If a folder is not available, the file will not be created.
     * \return True on success.
     */
    bool storeFrame(std::string filename);

    /**
     * Loads the jpeg and store its content to 'mFrameOutput'.
     */
    bool loadJpeg(std::string filename, uint32_t width, uint32_t height);

    // REQUIRED(?) jpeg methods.
 private:
    /* setup the buffer but we did that in the main function */
    static void init_buffer(jpeg_compress_struct* cinfo) {}
     
    /* what to do when the buffer is full; this should almost never
     * happen since we allocated our buffer to be big to start with
     */
    static boolean empty_buffer(jpeg_compress_struct* cinfo) {
	    return TRUE;
    }
     
    /* finalize the buffer and do any cleanup stuff */
    static void term_buffer(jpeg_compress_struct* cinfo) {}

    static void my_init_source(jpeg_decompress_struct* cinfo) {}
    static boolean my_fill_input_buffer(jpeg_decompress_struct* cinfo) {return true;}
    static void my_skip_input_data(jpeg_decompress_struct* cinfo, long num_bytes) {}
    static boolean my_resync_to_restart(jpeg_decompress_struct* cinfo, int desired) {return true;}
    static void my_term_source(jpeg_decompress_struct* cinfo) {}

 private:
    base::samples::frame::Frame mFrameOutput;

    unsigned char* mBuffer;
    unsigned int mBufferSize;

    struct jpeg_compress_struct cinfo;
	struct jpeg_decompress_struct dinfo; 
    struct jpeg_error_mgr jerr;
    struct jpeg_source_mgr mSrcMgr;
    struct jpeg_destination_mgr mDestMgr;

    int mJpegQuality;
};
} // end namespace conversion
#endif
