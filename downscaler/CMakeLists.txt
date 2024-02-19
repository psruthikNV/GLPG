set(GLPG_INCLUDES PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/sdk/d3d8/include)

set(DOWNSCALER_SOURCES downscaler.cpp downscaler_utils.cpp downscaler_decoder.cpp)

add_executable(downscaler ${DOWNSCALER_SOURCES})
target_include_directories(downscaler ${GLPG_INCLUDES})
target_link_libraries(downscaler glpg)
target_compile_features(downscaler PUBLIC cxx_std_17)
