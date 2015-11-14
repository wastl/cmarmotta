cmake_minimum_required(VERSION 3.0)
project(Marmotta)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

find_package (Threads)
find_package (RAPTOR)
find_package (Rasqal)
find_package (GRPC)
find_package (Protobuf REQUIRED)

add_subdirectory(model)
add_subdirectory(parser)
add_subdirectory(serializer)
add_subdirectory(test)

include_directories(. ${CMAKE_CURRENT_BINARY_DIR}/model)
include_directories(${PROTOBUF_INCLUDE_DIRS})

add_executable(main main.cc)
target_link_libraries(main marmotta_model marmotta_parser marmotta_serializer)
