#include <iostream>
#include <jpeg_conversion/jpeg_conversion.hpp>

int main(int argc, char** argv)
{
    using namespace conversion;

    if(argc != 4) {
        std::cout << "Usage: jpeg_conversion_test <jpeg_file> <jpeg_width> <jpeg_height>" 
                << std::endl;
        return 0;
    }

    JpegConversion conv;
    std::string file_name(argv[1]);
    int width = atoi(argv[2]);
    int height = atoi(argv[3]);    
    
    // Load jpeg.
    base::samples::frame::Frame frame_loaded;
    conv.loadJpeg(file_name, width, height, frame_loaded);

    size_t pos_dot = file_name.find_last_of(".");
    file_name.erase(pos_dot, file_name.size());

    base::samples::frame::Frame frame_output;
    
    // RGB
    frame_output.frame_mode = base::samples::frame::MODE_RGB;
    conv.decompress(frame_loaded, frame_output);
    conv.storeFile(file_name + "RGB", frame_output);

    // BGR
    frame_output.frame_mode = base::samples::frame::MODE_BGR;
    conv.decompress(frame_loaded, frame_output);
    conv.storeFile(file_name + "BGR", frame_output);

    // Grayscale
    frame_output.frame_mode = base::samples::frame::MODE_GRAYSCALE;
    conv.decompress(frame_loaded, frame_output);
    conv.storeFile(file_name + "GRAYSCALE", frame_output);

    // UYVY
    frame_output.frame_mode = base::samples::frame::MODE_UYVY;
    conv.decompress(frame_loaded, frame_output);
    conv.storeFile(file_name + "UYVY", frame_output);

	return 0;
}
