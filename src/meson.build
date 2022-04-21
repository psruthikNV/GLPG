src_files = files('GLPGContext_GL.cpp')

if host_machine.system() == 'linux'
  src_files += files('GLPGWindow_Xcb.cpp', 'GLPGWindowImpl_Xcb.cpp',
                     'GLPGContextImpl_GL_EGL.cpp', 'GLPGContextImpl_Platform_EGL.cpp',
                     'GLPGEvent_Xcb.cpp', 'GLPGEventImpl_Xcb.cpp')
elif host_machine.system() == 'windows'
  src_files += files('GLPGWindow_Win32.cpp', 'GLPGWindowImpl_Win32.cpp',
                     'GLPGContextImpl_GL_WGL.cpp', 'GLPGContextImpl_Platform_WGL.cpp',
                     'GLPGEvent_Win32.cpp', 'GLPGEventImpl_Win32.cpp',
                     'GLPGD3D12Context.cpp', 'GLPGD3D11Context.cpp', 'GLPGD3D9Context.cpp')
endif

libglpg = static_library('GLPG', src_files,
                         include_directories : glpg_include_path,
                         dependencies : dependenciesList)
