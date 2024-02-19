set(GLPG_INCLUDES PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/sdk/d3d8/include)

set(GLPG_SOURCES)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    list(APPEND GLPG_SOURCES
        GLPGWindow_Win32.cpp
        GLPGWindowImpl_Win32.cpp GLPGContextImpl_GL_WGL.cpp GLPGContext_GL.cpp
        GLPGContextImpl_Platform_WGL.cpp GLPGEvent_Win32.cpp
        GLPGEventImpl_Win32.cpp GLPGD3D12Context.cpp
        GLPGD3D11Context.cpp GLPGD3D9Context.cpp GLPGD3D8Context.cpp)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "LINUX")
    list(APPEND GLPG_SOURCES
        GLPGWindow_Xcb.cpp GLPGWindowImpl_Xcb.cpp GLPGContext_GL.cpp
        GLPGContextImpl_GL_EGL.cpp GLPGContextImpl_Platform_EGL.cpp
        GLPGEvent_Xcb.cpp GLPGEventImpl_Xcb.cpp)
else()
    message(SEND_ERROR "Unsupported platform!")
endif()

add_library(glpg STATIC ${GLPG_SOURCES})

target_include_directories(glpg ${GLPG_INCLUDES})
target_link_libraries(glpg ${GLPG_LIBRARIES})
target_compile_features(glpg PUBLIC cxx_std_17)
