rock_library(jpeg_conversion
    DEPS_PKGCONFIG base-types base-lib base-logging
    DEPS_CMAKE JPEG
    SOURCES jpeg_conversion.cpp
    HEADERS jpeg_conversion.hpp)

rock_executable(jpeg_conversion_tool
    SOURCES main.cpp    
    DEPS jpeg_conversion)


