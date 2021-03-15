vulkan_sdk_dir = ''

if host_machine.system() == 'windows'
  vulkan_sdk_dir = 'C:\\VulkanSDK\\1.2.170.0'
endif

vk_include_path = include_directories(vulkan_sdk_dir + '\include')
vk_lib_path = vulkan_sdk_dir + '\Lib'
dependenciesList += [glpg_compiler.find_library('vulkan-1', dirs: vk_lib_path, required: true)]

executable('vk_101', 'vk_101.cpp',
           include_directories : vk_include_path,
           dependencies : dependenciesList,
           link_with : libglpg)

