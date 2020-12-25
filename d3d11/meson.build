dependenciesList += [glpg_compiler.find_library('dxgi')]
dependenciesList += [glpg_compiler.find_library('d3d11')]
dependenciesList += [glpg_compiler.find_library('d3dcompiler')]

#TODO: Integrate FXC into build scripts to build shaders automatically at compile time
executable('d3d11_101', 'd3d11_101.cpp',
           dependencies : dependenciesList,
           link_with: libglpg)

