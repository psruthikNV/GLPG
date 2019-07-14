test_src = files('hello_triangle.cpp', 'native_window.cpp', 'opengl_context.cpp')

if host_machine.system() == 'linux'
    deps = [lib_xcb, lib_egl, lib_gl]
elif host_machine.system() == 'windows'
    deps = [lib_user32, lib_gl]
endif

executable('hello_triangle', test_src,
           include_directories : glpg_include_path,
           dependencies : deps)
