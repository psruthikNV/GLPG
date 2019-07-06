test_src = files('egl_test.cpp', 'native_window.cpp', 'opengl_context.cpp')

executable('glpg', test_src,
		   include_directories : glpg_include_path,
		   dependencies : [lib_xcb, lib_egl, lib_gl])
