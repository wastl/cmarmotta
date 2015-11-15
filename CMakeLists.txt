cmake_minimum_required(VERSION 3.0)
project(Marmotta)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
set(PROTOBUF_IMPORT_DIRS "${CMAKE_SOURCE_DIR}/model")

find_package (Threads)
find_package (RAPTOR)
find_package (Rasqal)
find_package (Protobuf REQUIRED)
find_package (GRPC REQUIRED)
find_package (LevelDB REQUIRED)

add_subdirectory(model)
add_subdirectory(service)
add_subdirectory(client)
add_subdirectory(parser)
add_subdirectory(serializer)
add_subdirectory(test)

include_directories(. ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/model)
include_directories(${PROTOBUF_INCLUDE_DIRS})

add_executable(main main.cc)
target_link_libraries(main marmotta_model marmotta_parser marmotta_serializer)
