dependenciesList += [glpg_compiler.find_library('dxguid')]

executable('d3d8test', 'test.cpp',
           include_directories : glpg_include_path,
           dependencies : dependenciesList,
           link_with: libglpg)
