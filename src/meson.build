test_src = files('GLPGWindow.cpp', 'GLPGContext.cpp', 'GLPGShaderUtils.cpp', 'GLPGEvent.cpp')

if host_machine.system() == 'linux'
    deps = [lib_xcb, lib_egl, lib_gl]
elif host_machine.system() == 'windows'
    deps = [lib_user32, lib_gl]
endif

libglpg = static_library('GLPG', test_src,
                         include_directories : glpg_include_path,
                         dependencies : deps)
