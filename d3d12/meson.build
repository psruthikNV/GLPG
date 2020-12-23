dependenciesList += [native_compiler.find_library('dxgi')]
dependenciesList += [native_compiler.find_library('d3d12')]
dependenciesList += [native_compiler.find_library('d3dcompiler')]

executable('d3d12_101', 'main.cpp',
           dependencies : dependenciesList),
           link_with: libglpg)

