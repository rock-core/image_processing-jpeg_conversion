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
   .define_method("load_jpeg", &conversion::JpegConversion::loadJpeg, (Arg("filename"), Arg("width"), Arg("height"), Arg("frame")))
   .define_method("store_frame", &do_store_frame, (Arg("filename"), Arg("frame")))
   ;
}
