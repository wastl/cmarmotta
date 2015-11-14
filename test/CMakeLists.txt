include_directories(.. ../model ${CMAKE_CURRENT_BINARY_DIR}/../model)

add_library(gtest STATIC gtest.h gtest-all.cc)

add_executable(model_tests StatementTest.cc main.cc)
target_link_libraries(model_tests gtest marmotta_model)

add_test(NAME ModelTest
         COMMAND model_tests)
