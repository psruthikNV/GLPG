dependenciesList += [glpg_compiler.find_library('dxgi')]
dependenciesList += [glpg_compiler.find_library('d3d11')]
dependenciesList += [glpg_compiler.find_library('d3dcompiler')]

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

