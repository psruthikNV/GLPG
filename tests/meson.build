# Use GTEST_BASE if GTEST libs are not present in /usr/lib
# TODO: Imporve this mess
GTEST_BASE = '/home/sruthik/googletest/'
GTEST_LIB_PATH = GTEST_BASE + 'build/lib'
GTEST_INCLUDE_PATH = GTEST_BASE + 'googletest/include'
GLM_INCLUDE_PATH = '/home/sruthik/glm/'

gtest_include_dir = include_directories(GTEST_INCLUDE_PATH)
glm_include_dir = include_directories(GLM_INCLUDE_PATH)

lib_gtest = glpg_compiler.find_library('gtest', dirs : GTEST_LIB_PATH)
lib_thread = dependency('threads')

executable('gtest_mock', 'gtest_mock.cpp',
           include_directories : [gtest_include_dir, glm_include_dir, glpg_include_path],
           dependencies : [lib_gtest, lib_thread])
