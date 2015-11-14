include_directories(.. ${CMAKE_CURRENT_BINARY_DIR}/../model)

add_library(marmotta_parser SHARED rdf_parser.h rdf_parser.cc)
set_target_properties(marmotta_parser PROPERTIES SOVERSION 1)
target_link_libraries(marmotta_parser marmotta_model ${CMAKE_THREAD_LIBS_INIT} ${RAPTOR_LIBRARY})