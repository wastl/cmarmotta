cmake_minimum_required(VERSION 3.0)
project(Marmotta)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")
set(PROTOBUF_IMPORT_DIRS "${CMAKE_SOURCE_DIR}/model")

find_package (Threads REQUIRED)
find_package (RAPTOR REQUIRED)
find_package (Rasqal REQUIRED)
find_package (GFlags REQUIRED)
find_package (Protobuf REQUIRED)
find_package (GRPC REQUIRED)
find_package (LevelDB REQUIRED)
find_package (GLog REQUIRED)

add_subdirectory(util)
add_subdirectory(model)
add_subdirectory(service)
add_subdirectory(client)
add_subdirectory(parser)
add_subdirectory(serializer)
add_subdirectory(sharding)
add_subdirectory(test)

