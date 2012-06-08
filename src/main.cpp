#include <iostream>
#include <jpeg_conversion/jpeg_conversion.hpp>

using namespace base::samples::frame;

int main(int argc, char** argv)
{
     if(argc != 4)
     {
         printf("usage: %s <filename> <height> <width>\n", argv[0]);
         return 0;
     }

     std::string filename(argv[1]);
     uint32_t height = atoi(argv[2]);
     uint32_t width = atoi(argv[3]);
     base::samples::frame::Frame frame;

     conversion::JpegConversion conversion;

     if(!conversion.loadJpeg(filename, width, height, frame))
     {
         printf("Failed to load jpeg from: %s\n", filename.c_str());
         return -1;
     } else {
         base::samples::frame::Frame outputFrame;
         conversion.decompress(frame, MODE_RGB, outputFrame);
          
         conversion.storeFrame(filename, outputFrame);
         printf("Converted to: %s\n", filename.c_str());
     }

     return 0;
}
