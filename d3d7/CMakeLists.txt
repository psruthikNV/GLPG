set(GLPG_INCLUDES PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/sdk/d3d8/include)

set(GLPG_SOURCES)

add_executable(test_d3d7 test_d3d7.cpp)

target_include_directories(test_d3d7 ${GLPG_INCLUDES})
target_link_libraries(test_d3d7 glpg)
target_compile_features(test_d3d7 PUBLIC cxx_std_17)
