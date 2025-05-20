set(GLPG_INCLUDES PRIVATE ${CMAKE_SOURCE_DIR}/include)

set(GLPG_SOURCES)

set(D3D12_SHADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/02_triangle_ps.hlsl
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/02_triangle_vs.hlsl
   )

set(HLSL_VS_PROFILE "vs_6_4")
set(HLSL_PS_PROFILE "ps_6_4")

foreach(shader ${D3D12_SHADER_FILES})
    get_filename_component(name ${shader} NAME_WE)
    string(FIND "${name}" "_vs" VERTEX_SHADER)
    string(FIND "${name}" "_ps" PIXEL_SHADER)
    set(infile ${shader})
    if (NOT VERTEX_SHADER EQUAL -1)
        set(outfile ${CMAKE_CURRENT_BINARY_DIR}/shaders/${name}.cso)
        add_custom_command(
          OUTPUT ${outfile}
          COMMAND dxc.exe -T ${HLSL_VS_PROFILE} -E main -Fo ${outfile} ${infile}
          DEPENDS ${infile}
          COMMENT "Compiling Vertex Shader: ${shader}"
          VERBATIM
        )
    elseif (NOT PIXEL_SHADER EQUAL -1)
        set(outfile ${CMAKE_CURRENT_BINARY_DIR}/shaders/${name}.cso)
        add_custom_command(
          OUTPUT ${outfile}
          COMMAND dxc.exe -T ${HLSL_PS_PROFILE} -E main -Fo ${outfile} ${infile}
          DEPENDS ${infile}
          COMMENT "Compiling Pixel Shader: ${shader}"
          VERBATIM
        )
    else()
        message(FATAL_ERROR "Unsupported Shader Type in '${shader}' (expected '_vs' or '_ps')")
    endif()
    list(APPEND COMPILED_SHADERS ${outfile})
endforeach()

list(SUBLIST COMPILED_SHADERS 0 2 02_triangle_shaders)
add_custom_target(02_triangle_shader_target ALL DEPENDS ${02_triangle_shaders})

add_executable(01_clear_color 01_clear_color.cpp)
target_include_directories(01_clear_color ${GLPG_INCLUDES})
target_link_libraries(01_clear_color glpg)
target_compile_features(01_clear_color PUBLIC cxx_std_17)

add_executable(02_triangle 02_triangle.cpp)
add_dependencies(02_triangle 02_triangle_shader_target)
target_include_directories(02_triangle ${GLPG_INCLUDES})
target_link_libraries(02_triangle glpg)
target_compile_features(02_triangle PUBLIC cxx_std_17)

