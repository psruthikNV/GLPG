dependenciesList += [glpg_compiler.find_library('dxgi')]
dependenciesList += [glpg_compiler.find_library('d3d12')]

dxc = find_program('dxc')
hlsl_vs_profile = 'vs_6_4'
hlsl_ps_profile = 'ps_6_4'

hlsl_shaders = ['shaders/02_triangle_vs.hlsl', 'shaders/02_triangle_ps.hlsl']

hlsl_shader_files = (hlsl_shaders)

shader_targets = []

foreach hlsl_shader : hlsl_shaders
    if hlsl_shader.contains('vs') == true
        shader_targets += custom_target(
            'shader @0@'.format(hlsl_shader),
            command : [dxc, '@INPUT@', '-T', hlsl_vs_profile, '-Fo', '@OUTPUT@'],
            input : hlsl_shader,
            output : '@BASENAME@'
        )
    elif hlsl_shader.contains('ps') == true
        shader_targets += custom_target(
            'shader @0@'.format(hlsl_shader),
            command : [dxc, '@INPUT@', '-T', hlsl_ps_profile, '-Fo', '@OUTPUT@'],
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