project('glpg', 'cpp')

glpg_include_path = include_directories('./include')
glpg_compiler = meson.get_compiler('cpp')

if host_machine.system() == 'linux'
    lib_xcb = glpg_compiler.find_library('xcb')
    lib_egl = glpg_compiler.find_library('EGL')
    lib_gl = glpg_compiler.find_library('GL')
endif

subdir('src')
