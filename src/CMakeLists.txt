rock_library(jpeg_conversion
    SOURCES Dummy.cpp
    HEADERS Dummy.hpp)

rock_executable(jpeg_conversion_bin Main.cpp
    DEPS jpeg_conversion)

