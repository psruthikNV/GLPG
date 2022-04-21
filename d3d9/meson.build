dependenciesList += [glpg_compiler.find_library('d3d9')]

executable('d3d9test', 'test.cpp',
           include_directories : d3d_include_path,
           dependencies : dependenciesList,
           link_with: libglpg)
