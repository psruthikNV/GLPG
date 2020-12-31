dependenciesList += [glpg_compiler.find_library('dxgi')]
dependenciesList += [glpg_compiler.find_library('d3d11')]

#TODO: Integrate FXC into build scripts to build shaders automatically at compile time

executable('01_clear_color', '01_clear_color.cpp',
           dependencies : dependenciesList,
           link_with: libglpg)

executable('02_triangle', '02_triangle.cpp',
           dependencies : dependenciesList,
           link_with: libglpg)

executable('03_model_matrix', '03_model_matrix.cpp',
           dependencies : dependenciesList,
           link_with: libglpg)

executable('04_index_buffer', '04_index_buffer.cpp',
           dependencies : dependenciesList,
           link_with: libglpg)

executable('05_instanced_draw', '05_instanced_draw.cpp',
           dependencies : dependenciesList,
           link_with: libglpg)

executable('06_camera_matrix', '06_camera_matrix.cpp',
           dependencies : dependenciesList,
           link_with: libglpg)

executable('07_projection_matrix', '07_projection_matrix.cpp',
           dependencies : dependenciesList,
           link_with: libglpg)
