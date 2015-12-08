include_directories(.. ${CMAKE_CURRENT_BINARY_DIR}/.. ${RAPTOR_INCLUDE_DIR}/raptor2)

add_library(marmotta_sparql rasqal_model.cc rasqal_model.h)
target_link_libraries(marmotta_sparql marmotta_model ${CMAKE_THREAD_LIBS_INIT} ${RASQAL_LIBRARY})