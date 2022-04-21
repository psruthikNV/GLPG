dependenciesList += [glpg_compiler.find_library('dxguid')]

executable('d3d7test', 'test.cpp',
           include_directories : d3d_include_path,
           dependencies : dependenciesList,
           link_with: libglpg)
