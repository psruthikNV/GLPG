project('glpg', 'cpp',
        version : '0.0.1')

glpg_include_path = include_directories('./include')
glpg_compiler = meson.get_compiler('cpp')
buildTests = get_option('buildTests')

if host_machine.system() == 'linux'
    lib_xcb = glpg_compiler.find_library('xcb')
    lib_egl = glpg_compiler.find_library('EGL')
    lib_gl = glpg_compiler.find_library('GL')
elif host_machine.system() == 'windows'
    lib_gl = glpg_compiler.find_library('opengl32')
    lib_user32 = glpg_compiler.find_library('user32')
endif

subdir('src')
subdir('examples')

if buildTests == true
    subdir('tests')
endif
