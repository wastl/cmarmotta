include_directories(
        .. ${CMAKE_CURRENT_BINARY_DIR}/..
        ${CMAKE_CURRENT_BINARY_DIR}/../model ${CMAKE_CURRENT_BINARY_DIR}/../service)

add_executable(marmotta_sharding sharding.cc sharding.h server.cc)
target_link_libraries(marmotta_sharding
        marmotta_util marmotta_model marmotta_service
        ${LevelDB_LIBRARY} ${GFLAGS_LIBRARY})

