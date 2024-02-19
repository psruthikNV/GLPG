set(GLPG_INCLUDES PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/sdk/d3d8/include)

set(GLPG_SOURCES)

add_executable(01_hello_triangle 01_hello_triangle.cpp)

target_include_directories(01_hello_triangle ${GLPG_INCLUDES})
target_link_libraries(01_hello_triangle glpg)
target_compile_features(01_hello_triangle PUBLIC cxx_std_17)

add_executable(01b_hello_coloured_triangle 01b_hello_coloured_triangle.cpp)

target_include_directories(01b_hello_coloured_triangle ${GLPG_INCLUDES})
target_link_libraries(01b_hello_coloured_triangle glpg)
target_compile_features(01b_hello_coloured_triangle PUBLIC cxx_std_17)

add_executable(02_model_matrix 02_model_matrix.cpp)

target_include_directories(02_model_matrix ${GLPG_INCLUDES})
target_link_libraries(02_model_matrix glpg)
target_compile_features(02_model_matrix PUBLIC cxx_std_17)

add_executable(03_element_array_buffer 03_element_array_buffer.cpp)

target_include_directories(03_element_array_buffer ${GLPG_INCLUDES})
target_link_libraries(03_element_array_buffer glpg)
target_compile_features(03_element_array_buffer PUBLIC cxx_std_17)

add_executable(04_camera_matrix 04_camera_matrix.cpp)

target_include_directories(04_camera_matrix ${GLPG_INCLUDES})
target_link_libraries(04_camera_matrix glpg)
target_compile_features(04_camera_matrix PUBLIC cxx_std_17)

add_executable(05_projection_matrix 05_projection_matrix.cpp)

target_include_directories(05_projection_matrix ${GLPG_INCLUDES})
target_link_libraries(05_projection_matrix glpg)
target_compile_features(05_projection_matrix PUBLIC cxx_std_17)

add_executable(06_monkey_indexed 06_monkey_indexed.cpp)

target_include_directories(06_monkey_indexed ${GLPG_INCLUDES})
target_link_libraries(06_monkey_indexed glpg)
target_compile_features(06_monkey_indexed PUBLIC cxx_std_17)

add_executable(07_triangle_texture 07_triangle_texture.cpp)

target_include_directories(07_triangle_texture ${GLPG_INCLUDES})
target_link_libraries(07_triangle_texture glpg)
target_compile_features(07_triangle_texture PUBLIC cxx_std_17)

add_executable(08_cube_texture 08_cube_texture.cpp)

target_include_directories(08_cube_texture ${GLPG_INCLUDES})
target_link_libraries(08_cube_texture glpg)
target_compile_features(08_cube_texture PUBLIC cxx_std_17)

add_executable(09a_render_to_texture 09a_render_to_texture.cpp)

target_include_directories(09a_render_to_texture ${GLPG_INCLUDES})
target_link_libraries(09a_render_to_texture glpg)
target_compile_features(09a_render_to_texture PUBLIC cxx_std_17)

add_executable(10_normal_lighting 10_normal_lighting.cpp)

target_include_directories(10_normal_lighting ${GLPG_INCLUDES})
target_link_libraries(10_normal_lighting glpg)
target_compile_features(10_normal_lighting PUBLIC cxx_std_17)

