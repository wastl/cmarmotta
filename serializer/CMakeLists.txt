include_directories(.. ${CMAKE_CURRENT_BINARY_DIR}/..)

add_library(marmotta_serializer rdf_serializer.h rdf_serializer.cc rdf_serializer.cc proto_serializer.cc proto_serializer.h base_serializer.cc base_serializer.h serializer.cc serializer.h)
target_link_libraries(marmotta_serializer marmotta_model ${CMAKE_THREAD_LIBS_INIT} ${RAPTOR_LIBRARY})