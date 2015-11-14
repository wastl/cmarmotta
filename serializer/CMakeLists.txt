include_directories(..)

add_library(marmotta_serializer SHARED rdf_serializer.h rdf_serializer.cc rdf_serializer.cc)
set_target_properties(marmotta_serializer PROPERTIES SOVERSION 1)
target_link_libraries(marmotta_serializer marmotta_model ${CMAKE_THREAD_LIBS_INIT} ${RAPTOR_LIBRARY})