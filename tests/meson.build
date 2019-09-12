glm_include_path = include_directories(get_option('glm_include_dir'))
lib_gtest = glpg_compiler.find_library('gtest')
lib_thread = dependency('threads')

executable('glpg_vector_tests', 'glpg_vector_tests.cpp',
           include_directories : [glpg_include_path, glm_include_path],
           dependencies : [lib_gtest, lib_thread])
