vulkan_sdk_dir = ''

glslc = find_program('glslc')

glsl_shaders = ['shaders/02_triangle_vs.glsl', 'shaders/02_triangle_fs.glsl']
glsl_shader_files = (glsl_shaders)

shader_targets = []

foreach glsl_shader : glsl_shaders
  if glsl_shader.contains('vs') == true
    shader_targets += custom_target(
      'shader @0@'.format(glsl_shader),
      command : [glslc, '-fshader-stage=vertex', '@INPUT@', '-o', '@OUTPUT@'],
      input : glsl_shader,
      output : '@BASENAME@'
    )
  elif glsl_shader.contains('fs') == true
    shader_targets += custom_target(
      'shader @0@'.format(glsl_shader),
      command : [glslc, '-fshader-stage=frag', '@INPUT@', '-o', '@OUTPUT@'],
      input : glsl_shader,
      output : '@BASENAME@'
    )
  endif
endforeach

if host_machine.system() == 'windows'
  vulkan_sdk_dir = 'C:\\VulkanSDK\\1.2.170.0'
endif

vk_include_path = include_directories(vulkan_sdk_dir + '\include')
vk_lib_path = vulkan_sdk_dir + '\Lib'
dependenciesList += [glpg_compiler.find_library('vulkan-1', dirs: vk_lib_path, required: true)]

executable('01_clear_color', '01_clear_color.cpp',
           include_directories : vk_include_path,
           dependencies : dependenciesList,
           link_with : libglpg)

executable('02_hello_triangle', ['02_hello_triangle.cpp', shader_targets[0], shader_targets[1]],
           include_directories : vk_include_path,
           dependencies : dependenciesList,
           link_with : libglpg)

