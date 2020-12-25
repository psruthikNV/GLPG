project('glpg', 'cpp',
        version : '0.0.2',
        default_options : 'cpp_std=c++17')

glpg_include_path = include_directories('./include')
glpg_compiler = meson.get_compiler('cpp')
buildTests = get_option('enable_tests')

if host_machine.system() == 'linux'
  dependenciesList = [glpg_compiler.find_library('xcb')]
  dependenciesList += [glpg_compiler.find_library('EGL'),
                       glpg_compiler.find_library('GL'),
                       glpg_compiler.find_library('xkbcommon'),
                       glpg_compiler.find_library('xkbcommon-x11')]

elif host_machine.system() == 'windows'
  dependenciesList = [glpg_compiler.find_library('opengl32')]
  dependenciesList += [glpg_compiler.find_library('user32')]
endif

subdir('src')
subdir('examples')
if buildTests == true
    subdir('tests')
endif
if host_machine.system() == 'windows'
  subdir('d3d11')
endif
