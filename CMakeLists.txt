# NOTE: Use 32bit builds when using D3D2 - D3D8
# cmake -T v142,host=x64 -G "Visual Studio 16 2019" -A Win32 ../

cmake_minimum_required(VERSION 3.10)

project(GLPG)

#set(VK_SDK_PATH "" CACHE STRING "Path to Vulkan SDK to use")
#if (VK_SDK_PATH STREQUAL "")
#    message(SEND_ERROR "VK_SDK_PATH not set!")
#endif()

set(CMAKE_CXX_FLAGS_DEBUG     "${CMAKE_CXX_FLAGS_DEBUG} -DGLPG_IS_DEBUG")
SET(CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS} /D \"SAFESEH:NO\"")
SET(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")

set(GLPG_LIBRARIES)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    list(APPEND GLPG_LIBRARIES
        PRIVATE user32
        PRIVATE opengl32
        PRIVATE d3d9
        PRIVATE dxguid
        PRIVATE ddraw
        PRIVATE ${CMAKE_SOURCE_DIR}/sdk/d3d8/lib/d3d8.lib)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "LINUX")
    list(APPEND GLPG_LIBRARIES
        PRIVATE xcb
        PRIVATE EGL
        PRIVATE GL
        PRIVATE xkbcommon
        PRIVATE xkbcommon-x11)
else()
    message(SEND_ERROR "Unsupported platform!")
endif()

add_subdirectory(src)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_subdirectory(d3d2)
    add_subdirectory(d3d7)
    add_subdirectory(d3d8)
    add_subdirectory(d3d9)
#    add_subdirectory(d3d11)
#    add_subdirectory(d3d12)
endif()
