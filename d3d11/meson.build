dependenciesList += [glpg_compiler.find_library('dxgi')]
dependenciesList += [glpg_compiler.find_library('d3d11')]

fxc = find_program('fxc')
hlsl_vs_profile = 'vs_4_0_level_9_1'
hlsl_ps_profile = 'ps_4_0_level_9_1'

hlsl_shaders = ['02_triangle_vs.hlsl', '02_triangle_ps.hlsl',
                '03_model_matrix_vs.hlsl', '03_model_matrix_ps.hlsl',
                '05_instanced_draw_vs.hlsl', '05_instanced_draw_ps.hlsl',
                '06_camera_matrix_vs.hlsl', '06_camera_matrix_ps.hlsl',
                '07_projection_matrix_vs.hlsl', '07_projection_matrix_ps.hlsl']

hlsl_shader_files = (hlsl_shaders)

shader_targets = []

foreach hlsl_shader : hlsl_shaders
    if hlsl_shader.contains('vs') == true
        shader_targets += custom_target(
            'shader @0@'.format(hlsl_shader),
            command : [fxc, '@INPUT@', '-T', hlsl_vs_profile, '-Fo', '@OUTPUT@'],
            input : hlsl_shader,
            output : '@BASENAME@'
        )
    elif hlsl_shader.contains('ps') == true
        shader_targets += custom_target(
            'shader @0@'.format(hlsl_shader),
            command : [fxc, '@INPUT@', '-T', hlsl_ps_profile, '-Fo', '@OUTPUT@'],
            input : hlsl_shader,
            output : '@BASENAME@'
        )
    endif
endforeach

executable('01_clear_color', '01_clear_color.cpp',
           dependencies : dependenciesList,
           link_with: libglpg)

executable('02_triangle', ['02_triangle.cpp', shader_targets[0], shader_targets[1]],
           dependencies : dependenciesList,
           link_with: libglpg)

executable('03_model_matrix', ['03_model_matrix.cpp', shader_targets[2], shader_targets[3]],
           dependencies : dependenciesList,
           link_with: libglpg)

executable('04_index_buffer', ['04_index_buffer.cpp', shader_targets[2], shader_targets[3]],
           dependencies : dependenciesList,
           link_with: libglpg)

executable('05_instanced_draw', ['05_instanced_draw.cpp', shader_targets[4], shader_targets[5]],
           dependencies : dependenciesList,
           link_with: libglpg)

executable('06_camera_matrix', ['06_camera_matrix.cpp', shader_targets[6], shader_targets[7]],
           dependencies : dependenciesList,
           link_with: libglpg)

executable('07_projection_matrix', ['07_projection_matrix.cpp', shader_targets[8], shader_targets[9]],
           dependencies : dependenciesList,
           link_with: libglpg)
