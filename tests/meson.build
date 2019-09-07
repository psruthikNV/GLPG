# Use GTEST_BASE if GTEST libs are not present in /usr/lib
# TODO: Imporve this mess
GTEST_BASE='/home/sruthik/googletest/'
GTEST_LIB_PATH=GTEST_BASE + 'build/lib'
GTEST_INCLUDE_PATH=GTEST_BASE + 'googletest/include'

gtest_include_dir = include_directories(GTEST_INCLUDE_PATH)

lib_gtest = glpg_compiler.find_library('gtest', dirs : GTEST_LIB_PATH)
lib_pthread = dependency('threads')

executable('gtest_mock', 'gtest_mock.cpp',
           include_directories : gtest_include_dir,
           dependencies : [lib_gtest, lib_pthread])
