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
    conv.loadJpeg(file_name, width, height);
    base::samples::frame::Frame frame_loaded = conv.getFrame();

    // Convert jpeg to rgba nd store as file_name.ppm.
    conv.decompress(frame_loaded);
    std::string name_rgb(argv[1]);
    std::cout << "name: " << name_rgb << std::endl;
    size_t pos_dot = name_rgb.find_last_of(".");
    std::cout << "remove from " << pos_dot << std::endl;
    name_rgb.erase(pos_dot, name_rgb.size());
    conv.storeFrame(name_rgb);

    // Compress rgb back to jpg and store as file_name2.jpg
    base::samples::frame::Frame frame_rgb = conv.getFrame();
    conv.compress(frame_rgb);
    conv.storeFrame(name_rgb + "2.jpg");

	return 0;
}
