#include "rice/Class.hpp"
#include "rice/Constructor.hpp"
#include "rice/String.hpp"
#include "rice/Array.hpp"

#include <stdint.h>
#include <vector>
#include <iostream>
#include <jpeg_conversion/jpeg_conversion.hpp>

using namespace Rice;

Rice::Data_Type<conversion::JpegConversion> rb_cJpegConversion;
typedef std::vector<uint8_t> ImageBuffer;

template<>
Object to_ruby< ImageBuffer >(const ImageBuffer& data)
{
    Array array;
    ImageBuffer::const_iterator it = data.begin();
    for(; it != data.end(); ++it)
    {
        array.push(*it);
    }
    return array;
}

Array do_from_file(Object self, String filename, int width, int height)
{
    Data_Object<conversion::JpegConversion> conversion(self, rb_cJpegConversion);
    std::string name = filename.str();

    //Data_Object<base::samples::frame::Frame> frame(new base::samples::frame::Frame(width, height));
    base::samples::frame::Frame frame;
    conversion->loadJpeg(name, width, height, frame);
    // std::vector<uint8_t>
    return to_ruby< ImageBuffer >(frame.image);
}

Object do_store_frame(Object self, String filename, Object roptr_frame)
{
    Data_Object<conversion::JpegConversion> conversion(self, rb_cJpegConversion);
    std::string name = filename.str();

    Data_Object<base::samples::frame::Frame*> frame(roptr_frame);

    conversion->storeFrame(name, **frame.get(), NULL);
    return conversion;
}

/**
* Initialise method in order to 
*/
extern "C"
void Init_jpeg_conversion_ext()
{
    // Define module JpegConversion
    Rice::Module rb_mJpegConversion = define_module("Conversion");

     rb_cJpegConversion = define_class_under<conversion::JpegConversion>(rb_mJpegConversion, "JpegConversion")
   .define_constructor(Constructor<conversion::JpegConversion, unsigned int>(), Arg("jpeg quality"))
   .define_constructor(Constructor<conversion::JpegConversion>())
   .define_method("do_from_file", &do_from_file, (Arg("filename"), Arg("width"), Arg("height")))
   .define_method("store_frame", &do_store_frame, (Arg("filename"), Arg("frame")))
   ;
}
