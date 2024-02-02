set(GLPG_INCLUDES PRIVATE ${CMAKE_SOURCE_DIR}/include)

set(GLPG_SOURCES)

set(D3D2_LIBRARIES glpg)

add_executable(test_d3d2 test_d3d2.cpp)

target_include_directories(test_d3d2 ${GLPG_INCLUDES})
target_link_libraries(test_d3d2 ${GLPG_LIBRARIES} ${D3D2_LIBRARIES})
target_compile_features(test_d3d2 PUBLIC cxx_std_17)
