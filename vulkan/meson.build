vulkan_sdk_dir = ''

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

executable('02_hello_triangle', '02_hello_triangle.cpp',
           include_directories : vk_include_path,
           dependencies : dependenciesList,
           link_with : libglpg)

