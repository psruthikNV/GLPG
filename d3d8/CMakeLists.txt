set(GLPG_INCLUDES PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/sdk/d3d8/include)

set(GLPG_SOURCES)

add_executable(d3d8_test d3d8_test.cpp)

target_include_directories(d3d8_test ${GLPG_INCLUDES})
target_link_libraries(d3d8_test glpg)
target_compile_features(d3d8_test PUBLIC cxx_std_17)
