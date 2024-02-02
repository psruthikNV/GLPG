set(GLPG_INCLUDES PRIVATE ${CMAKE_SOURCE_DIR}/include)

set(GLPG_SOURCES)

add_executable(test_d3d9 test_d3d9.cpp)

target_include_directories(test_d3d9 ${GLPG_INCLUDES})
target_link_libraries(test_d3d9 glpg)
target_compile_features(test_d3d9 PUBLIC cxx_std_17)
