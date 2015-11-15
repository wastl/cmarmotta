include_directories(.. ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/../model)

file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/*.proto")
PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${ProtoFiles})
PROTOBUF_GENERATE_GRPC_CPP(GRPC_SRCS GRPC_HDRS ${ProtoFiles})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(marmotta_service SHARED ${PROTO_SRCS} ${PROTO_HDRS} ${GRPC_SRCS} ${GRPC_HDRS})
set_target_properties(marmotta_service PROPERTIES SOVERSION 1)
target_link_libraries(marmotta_service marmotta_model ${CMAKE_THREAD_LIBS_INIT} ${PROTOBUF_LIBRARIES} ${GRPC_LIBRARIES})

add_executable(marmotta_persistence persistence.cc persistence.h server.cc)
target_link_libraries(marmotta_persistence marmotta_model marmotta_service ${LevelDB_LIBRARY})

