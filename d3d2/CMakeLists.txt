set(GLPG_INCLUDES PRIVATE ${CMAKE_SOURCE_DIR}/include)

set(GLPG_SOURCES)

#set(D3D2_LIBRARIES dxguid ddraw)

add_executable(test test.cpp)

target_include_directories(test ${GLPG_INCLUDES})
target_link_libraries(test ${GLPG_LIBRARIES})
target_compile_features(test PUBLIC cxx_std_17)
