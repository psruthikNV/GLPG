dependenciesList += [glpg_compiler.find_library('dxgi')]
dependenciesList += [glpg_compiler.find_library('d3d12')]

dxc = find_program('dxc')

executable('01_clear_color', '01_clear_color.cpp',
           dependencies : dependenciesList,
           link_with: libglpg)

