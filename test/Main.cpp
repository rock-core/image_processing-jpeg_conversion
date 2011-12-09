#include <iostream>
#include <jpeg_conversion/jpeg_conversion.hpp>

int main(int argc, char** argv)
{
    using namespace conversion;

    if(argc != 5) {
        std::cout << "Usage: jpeg_conversion_test <jpeg_file> <jpeg_width> <jpeg_height> <jpeg_quality>" 
                << std::endl;
        return 0;
    }

    std::string file_name(argv[1]);
    int width = atoi(argv[2]);
    int height = atoi(argv[3]);    
    int quality = atoi(argv[4]);  

    JpegConversion conv(quality);
    
    // Load jpeg.
    base::samples::frame::Frame frame_loaded;
    conv.loadJpeg(file_name, width, height, frame_loaded);

    conv.loadJpeg(file_name, width, height, frame_loaded);

    size_t pos_dot = file_name.find_last_of(".");
    file_name.erase(pos_dot, file_name.size());

    base::samples::frame::Frame frame_decompressed;
    base::samples::frame::Frame frame_compressed;

    // BGR
    // jpeg -> bgr
    try {
        std::cout << "JPEG to BGR" << std::endl;
        conv.decompress(frame_loaded, base::samples::frame::MODE_BGR, frame_decompressed);
        conv.storeFrame(file_name + "BGR", frame_decompressed);
        // bgr -> jpeg
        std::cout << "BGR to JPEG" << std::endl;
        conv.compress(frame_decompressed, frame_compressed);
        conv.storeFrame(file_name + "BGR", frame_compressed);
    } catch(std::runtime_error& e) {
        std::cout << "JPEG/BGR conversion: " << e.what() << std::endl;
    }

    // Grayscale
    // jpeg -> grayscale
    try {
        std::cout << "JPEG to GRAYSCALE" << std::endl;
        conv.decompress(frame_loaded, base::samples::frame::MODE_GRAYSCALE, frame_decompressed);
        conv.storeFrame(file_name + "GRAYSCALE", frame_decompressed);
        // grayscale -> jpeg
        std::cout << "GRAYSCALE to JPEG" << std::endl;
        conv.compress(frame_decompressed, frame_compressed);
        conv.storeFrame(file_name + "GRAYSCALE", frame_compressed);
    } catch(std::runtime_error& e) {
        std::cout << "JPEG/GRAYSCALE conversion: " << e.what() << std::endl;
    }

    // RGB
    // jpeg -> rgb
    try {
        std::cout << "JPEG to RGB" << std::endl;
        frame_decompressed.init(width, height, 8, base::samples::frame::MODE_RGB);
        //conv.decompress(frame_loaded, base::samples::frame::MODE_RGB, frame_decompressed);
        conv.decompress(frame_loaded.getImageConstPtr(), 
            frame_loaded.getNumberOfBytes(),
            frame_loaded.getWidth(),
            frame_loaded.getHeight(),
            base::samples::frame::MODE_RGB, 
            frame_decompressed.getImagePtr());
        conv.storeFrame(file_name + "RGB", frame_decompressed);
        // rgb -> jpeg
        std::cout << "RGB to JPEG" << std::endl;
        conv.compress(frame_decompressed, frame_compressed);
        conv.storeFrame(file_name + "RGB", frame_compressed);
    } catch(std::runtime_error& e) {
        std::cout << "JPEG/RGB conversion: " << e.what() << std::endl;
    }

	return 0;
}
