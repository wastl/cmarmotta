include_directories(.. ${CMAKE_CURRENT_BINARY_DIR}/..)

add_library(marmotta_serializer rdf_serializer.h rdf_serializer.cc rdf_serializer.cc)
target_link_libraries(marmotta_serializer marmotta_model ${CMAKE_THREAD_LIBS_INIT} ${RAPTOR_LIBRARY})