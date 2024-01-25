set(GLPG_INCLUDES PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/sdk/d3d8/include)

set(GLPG_SOURCES)

add_executable(01_clear_color 01_clear_color.cpp)

target_include_directories(01_clear_color ${GLPG_INCLUDES})
target_link_libraries(01_clear_color glpg)
target_compile_features(01_clear_color PUBLIC cxx_std_17)

add_executable(02_hello_triangle 02_hello_triangle.cpp)

target_include_directories(02_hello_triangle ${GLPG_INCLUDES})
target_link_libraries(02_hello_triangle glpg)
target_compile_features(02_hello_triangle PUBLIC cxx_std_17)
