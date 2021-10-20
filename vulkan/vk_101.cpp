#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#else
#define VK_USE_PLATFORM_XCB_KHR
#endif
#include <vulkan/vulkan.hpp>

#include "../include/GLPGWindow.hpp"
#include "../include/GLPGEvent.hpp"
#include "../include/utils/vk-print.hpp"
#include "../include/utils/vk-utils.hpp"
#undef CreateWindow

int main()
{
    GLPG::GLPGEventLoop eventLoop;
    GLPG::GLPGEvent event;
    constexpr uint32_t defaultDeviceIdx = 0U;
    constexpr int32_t width = 2560;
    constexpr int32_t height = 1440;
    uint32_t numPhysicalDevices = 0U;
    uint32_t numInstanceExtensions = 0U;
    uint32_t numDeviceExtensions = 0U;
    uint32_t numDeviceQueues = 0U;
    const char *requiredInstanceExtns[] = {
        "VK_KHR_display",
        "VK_KHR_surface",
#ifdef _WIN32
        "VK_KHR_win32_surface",
#else
        "VK_KHR_xcb_surface",
#endif
#if GLPG_IS_DEBUG
        "VK_EXT_debug_utils",
#endif
    };
    const char *requiredDeviceExtns[] = {
        "VK_KHR_swapchain"
    };
    
#if GLPG_IS_DEBUG
    const char *requiredLayers[] = {
        "VK_LAYER_KHRONOS_validation"
    };
#endif

    constexpr uint32_t numRequiredInstanceExtns = array_size(requiredInstanceExtns);
    constexpr uint32_t numRequiredDeviceExtns = array_size(requiredDeviceExtns);
    std::cout << "Array size: " << array_size(requiredInstanceExtns) << "\n";

    GLPG::GLPGWindow *window = GLPG::GLPGWindow::GetInstance();
    if (!window) {
        std::cerr << "Faield to get GLPGWindow instance\n";
        return -1;
    }

    if (window->CreateWindow(width, height)) {
        std::cout << "Width x Height: " << window->GetWindowWidth() << "x" << window->GetWindowHeight() << "\n";
    } else {
        std::cout << "Failed to create native window\n";
        return -1;
    }

    if (!eventLoop.InitializeEventLoop()) {
        std::cerr << "Failed to initialize event loop\n";
        return -1;
    }

#ifdef _WIN32
    HWND *hWindow = reinterpret_cast<HWND *>(window->GetWindowHandle());
    HINSTANCE hInstance = GetModuleHandle(NULL);
#endif

    VkResult result;
    VkInstance instance;
    VkDevice device;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;

    VkPhysicalDevice *physicalDevicesArray;
    VkPhysicalDeviceProperties *physicalDevicePropertiesArray;
    VkExtensionProperties *instanceExtnPropArray;
    VkExtensionProperties *deviceExtnPropArray;
    VkQueueFamilyProperties *queuePropArray;

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.enabledExtensionCount = numRequiredInstanceExtns;
    instanceCreateInfo.ppEnabledExtensionNames = requiredInstanceExtns;
#if GLPG_IS_DEBUG
    instanceCreateInfo.enabledLayerCount = array_size(requiredLayers);
    instanceCreateInfo.ppEnabledLayerNames = requiredLayers;
#endif

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    float queuePriority = 1.0F;
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueCount = 1U;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1U;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = numRequiredDeviceExtns;
    deviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtns;
    

#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfo = {};
    win32SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    win32SurfaceCreateInfo.hinstance = hInstance;
    win32SurfaceCreateInfo.hwnd = *hWindow;
#endif

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
    swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapchainCreateInfo.imageArrayLayers = 1U;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    VkSurfaceCapabilitiesKHR surfaceCaps = {};
    uint32_t layerPropCount = 0U;
    VkLayerProperties layerProps[17];

    if (vkEnumerateInstanceLayerProperties(&layerPropCount, nullptr) != VK_SUCCESS) {
        std::cerr << "Failed to get layer prop count\n";
    } else {
        std::cerr << "Instance layers: " << layerPropCount << "\n";
    }

    if (vkEnumerateInstanceLayerProperties(&layerPropCount, layerProps) != VK_SUCCESS) {
        std::cerr << "Failed to enumerate layer props\n";
    } else {
        for (uint32_t idx = 0U; idx < layerPropCount; idx++) {
            VkPrintVkLayerProperties(layerProps[idx]);
            std::cout << "\n";
        }
    }

    if (auto ret = vkCreateInstance(&instanceCreateInfo, nullptr, &instance); ret != VK_SUCCESS) {
        std::cerr << "vkCreateInstance failed: " << ret << "\n";
        return -1;
    }

    if (vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, nullptr) != VK_SUCCESS) {
        fprintf(stderr, "Failed to get the number of physical devices present\n");
        return -1;
    } else {
        fprintf(stderr, "Number of physical devices: %u\n", numPhysicalDevices);
    }

    physicalDevicesArray = new VkPhysicalDevice[numPhysicalDevices];

    if (vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, physicalDevicesArray) != VK_SUCCESS) {
        fprintf(stderr, "Failed to get Physical Device instance\n");
        return -1;
    }

    physicalDevicePropertiesArray = new VkPhysicalDeviceProperties[numPhysicalDevices];

    for (uint32_t idx = 0U; idx < numPhysicalDevices; idx++) {
        vkGetPhysicalDeviceProperties(physicalDevicesArray[idx], &physicalDevicePropertiesArray[idx]);
        fprintf(stderr, "Physical Device Properties for Device %u\n", idx);
        fprintf(stderr, "Device Name: %s\n", physicalDevicePropertiesArray[idx].deviceName);
        fprintf(stderr, "Vulkan API Version: %u.%u.%u\n", VK_VERSION_MAJOR(physicalDevicePropertiesArray[idx].apiVersion),
                                                          VK_VERSION_MINOR(physicalDevicePropertiesArray[idx].apiVersion),
                                                          VK_VERSION_PATCH(physicalDevicePropertiesArray[idx].apiVersion));
    }

    if (vkEnumerateInstanceExtensionProperties(nullptr, &numInstanceExtensions, nullptr) != VK_SUCCESS) {
        fprintf(stderr, "Failed to query number of instance extensions\n");
        return -1;
    } else {
        fprintf(stderr, "Number of Instance Extensions: %u\n", numInstanceExtensions);
    }

    instanceExtnPropArray = new VkExtensionProperties[numInstanceExtensions];

    if (vkEnumerateInstanceExtensionProperties(nullptr, &numInstanceExtensions, instanceExtnPropArray) != VK_SUCCESS) {
        fprintf(stderr, "Failed to query instance extensions\n");
        return -1;
    } else {
        for (uint32_t idx = 0U; idx < numInstanceExtensions; idx++) {
            fprintf(stderr, "%s : %u\n", instanceExtnPropArray[idx].extensionName, instanceExtnPropArray[idx].specVersion);
        }
    }

    if (vkEnumerateDeviceExtensionProperties(physicalDevicesArray[defaultDeviceIdx], nullptr, &numDeviceExtensions, nullptr) != VK_SUCCESS) {
        fprintf(stderr, "Failed to query number of device extensions\n");
        return -1;
    } else {
        fprintf(stderr, "Number of Device Extensions: %u\n", numDeviceExtensions);
    }

    deviceExtnPropArray = new VkExtensionProperties[numDeviceExtensions];

    if (vkEnumerateDeviceExtensionProperties(physicalDevicesArray[defaultDeviceIdx], nullptr, &numDeviceExtensions, deviceExtnPropArray) != VK_SUCCESS) {
        fprintf(stderr, "Failed to query number of device extensions\n");
        return -1;
    } else {
        for (uint32_t idx = 0U; idx < numDeviceExtensions; idx++) {
            fprintf(stderr, "%s : %u\n", deviceExtnPropArray[idx].extensionName, deviceExtnPropArray[idx].specVersion);
        }
    }

    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevicesArray[defaultDeviceIdx], &numDeviceQueues, nullptr);
    fprintf(stderr, "Number of Device Queues: %u\n", numDeviceQueues);

    queuePropArray = new VkQueueFamilyProperties[numDeviceQueues];
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevicesArray[defaultDeviceIdx], &numDeviceQueues, queuePropArray);
    std::cout << "\t VkQueueFamilyProperties: \n";
    ParseDeviceQueueDetails(numDeviceQueues, queuePropArray);

    if (vkCreateDevice(physicalDevicesArray[defaultDeviceIdx], &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create VkDevice\n");
        return -1;
    }
    VkQueue queue;
    vkGetDeviceQueue(device, 0, 0, &queue);

    VkCommandPool cmdPool;
    VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
    cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmdPoolCreateInfo.queueFamilyIndex = 0U;

    if (vkCreateCommandPool(device, &cmdPoolCreateInfo, nullptr, &cmdPool) != VK_SUCCESS) {
        std::cerr << "Failed to create command pool\n";
        return -1;
    }

    VkCommandBuffer cmdBuffer;
    VkCommandBufferAllocateInfo cmdBufferAllocateInfo = {};
    cmdBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufferAllocateInfo.commandPool = cmdPool;
    cmdBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufferAllocateInfo.commandBufferCount = 1U;

    if (vkAllocateCommandBuffers(device, &cmdBufferAllocateInfo, &cmdBuffer) != VK_SUCCESS) {
        std::cerr << "Failed to allocate command buffer\n";
    }

#ifdef _WIN32
    if (vkCreateWin32SurfaceKHR(instance, &win32SurfaceCreateInfo, nullptr, &surface) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create VkSurface\n");
        return -1;
    }
#endif

    VkBool32 surfaceSupport = VK_FALSE;
    if (vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevicesArray[defaultDeviceIdx], 0U, surface, &surfaceSupport) != VK_SUCCESS) {
        std::cerr << "vkGetPhysicalDeviceSurfaceSupportKHR failed\n";
        return -1;
    } else {
        if (surfaceSupport != VK_TRUE) {
            std::cerr << "Queue does not support presents\n";
            return -1;
        }
    }
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevicesArray[defaultDeviceIdx], surface,
                                                  &surfaceCaps) != VK_SUCCESS) {
        std::cerr << "Failed to get Surface capabilities\n";
    } else {
        std::cout << "\t VkSurfaceCapabilitiesKHR: \n";
        VkPrintVkSurfaceCapabilitiesKHR(surfaceCaps);
        swapchainCreateInfo.imageExtent = surfaceCaps.currentExtent;
        swapchainCreateInfo.minImageCount = surfaceCaps.minImageCount;
    }

    uint32_t numSupportedSwapchainColorFormats = 0U;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevicesArray[defaultDeviceIdx], surface, &numSupportedSwapchainColorFormats, nullptr) != VK_SUCCESS) {
        std::cerr << "Failed to get the number of color formats supported by the swapchain\n";
        return -1;
    } else {
        VkSurfaceFormatKHR supportedSurfaceColorFormats[3];
        std::cerr << "Number of supported color formats: " << numSupportedSwapchainColorFormats << "\n";
         if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevicesArray[defaultDeviceIdx], surface, &numSupportedSwapchainColorFormats,
                                                  supportedSurfaceColorFormats) != VK_SUCCESS) {
             std::cerr << "Failed to get the color formats supported by the swapchain\n";
             return -1;
         } else {
             std::cerr << "Supported swapchain formats : \n";
             for (uint32_t idx = 0U; idx < numSupportedSwapchainColorFormats; idx++) {
                 VkPrintVkSurfaceFormatKHR(supportedSurfaceColorFormats[idx]);
             }
         }
    }

    swapchainCreateInfo.surface = surface;

    if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create VkSwapchain\n");
        return -1;
    }

    uint32_t swapchainImageCount = 0U;
    VkImage swapchainImages[2];
    if (vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr) != VK_SUCCESS) {
        std::cerr << "Failed to get number of swapchain images\n";
        return -1;
    } else {
        std::cerr << "Number of swapchain images: " << swapchainImageCount << "\n";
    }

    if (vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages) != VK_SUCCESS) {
        std::cerr << "Failed to acquire swapchain images\n";
        return -1;
    }

    uint32_t backbufferIdx = 0U;
    if (vkAcquireNextImageKHR(device, swapchain, 0U, VK_NULL_HANDLE, VK_NULL_HANDLE,
                              &backbufferIdx) != VK_SUCCESS) {
        std::cerr << "Failed to get backbuffer index\n";
    } else {
        std::cerr << "Backbuffer idx: " << backbufferIdx << "\n";
    }

    VkClearColorValue clearColorValue = {};
    uint32_t clearColorValues[4] = {0U, 0U, 255U, 0U};
    clearColorValue.uint32[0] = 0U;
    VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
    VkSubmitInfo queueSubmitInfo = {};
    queueSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    queueSubmitInfo.commandBufferCount = 1U;
    queueSubmitInfo.pCommandBuffers = &cmdBuffer;
    cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    while ((event = eventLoop.GetEvent()) != GLPG::GLPGEvent::Key_Escape) {
        if (vkAcquireNextImageKHR(device, swapchain, 0U, VK_NULL_HANDLE, VK_NULL_HANDLE,
                              &backbufferIdx) != VK_SUCCESS) {
            std::cerr << "Failed to get backbuffer index\n";
        }
        if (vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo) != VK_SUCCESS) {
            std::cerr << "Failed to translate cmd buffer to begin state\n";
            return -1;
        }
        vkCmdClearColorImage(cmdBuffer, swapchainImages[backbufferIdx], VK_IMAGE_LAYOUT_GENERAL,
                             &clearColorValue, 1, nullptr);
        if (vkQueueSubmit(queue, 1U, &queueSubmitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
            std::cerr << "Queue submission failed\n";
            return -1;
        }
        
    }
    delete[] physicalDevicesArray;
    delete[] physicalDevicePropertiesArray;
    delete[] instanceExtnPropArray;
    delete[] deviceExtnPropArray;
    delete[] queuePropArray;
}
