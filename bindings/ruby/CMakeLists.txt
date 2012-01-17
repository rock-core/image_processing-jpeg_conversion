include(RockRuby)
rock_find_pkgconfig(BASE_TYPES REQUIRED base-types)
rock_ruby_rice_extension(jpeg_conversion_ext jpeg_conversion.cpp)
target_link_libraries(jpeg_conversion_ext ${BASE_TYPES_LIBRARIES})
rock_ruby_library(jpeg_conversion)

