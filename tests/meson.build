glm_include_path = include_directories(get_option('glm_include_dir'))
lib_gtest = glpg_compiler.find_library('gtest')
lib_thread = dependency('threads')

executable('gtest_mock', 'gtest_mock.cpp',
           include_directories : [glpg_include_path, glm_include_path],
           dependencies : [lib_gtest, lib_thread])
