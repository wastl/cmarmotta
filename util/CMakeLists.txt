include_directories(.. ${CMAKE_CURRENT_BINARY_DIR}/..)

add_library(marmotta_util SHARED murmur3.cc murmur3.h)
set_target_properties(marmotta_util PROPERTIES SOVERSION 1)