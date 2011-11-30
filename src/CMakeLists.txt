rock_library(jpeg_conversion
    DEPS_PKGCONFIG base-types
    DEPS_CMAKE JPEG
    SOURCES jpeg_conversion.cpp
    HEADERS jpeg_conversion.hpp)

rock_executable(jpeg_conversion_test Main.cpp
    DEPS jpeg_conversion)



