#ifdef WIN32
#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#else
#define VK_USE_PLATFORM_XCB_KHR
#include <vulkan/vulkan.hpp>

#ifdef WIN32
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
    LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam,
    LPARAM lParam) {

    switch (message) {
    case WM_PAINT:
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

HWND CreateWin32Window(const int32_t width, const int32_t height) {
    const char *windowName = "vk_101";
    int a;
    int b;
    WNDCLASSEX wndClass = { 0 };

    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WindowProc;
    wndClass.hInstance = GetModuleHandle(NULL);
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.lpszClassName = "vk_101";

    ATOM ret = RegisterClassEx(&wndClass);

    RECT windowRect = { 0, 0, width, height };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    return (CreateWindow(wndClass.lpszClassName, windowName,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top, nullptr, nullptr,
        wndClass.hInstance, nullptr));
}
#endif

void ParseDeviceQueueDetails(uint32_t numDeviceQueues, VkQueueFamilyProperties *queuePropArray)
{
    for (uint32_t idx = 0U; idx < numDeviceQueues; idx++) {
        fprintf(stderr, "Queue Family %u:\n", idx);
        fprintf(stderr, "Num queues: %u\n", queuePropArray[idx].queueCount);
        fprintf(stderr, "Supported caps:\n");
        if (queuePropArray[idx].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            fprintf(stderr, "VK_QUEUE_GRAPHICS_BIT\n");
        }
        if (queuePropArray[idx].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            fprintf(stderr, "VK_QUEUE_COMPUTE_BIT\n");
        }
        if (queuePropArray[idx].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            fprintf(stderr, "VK_QUEUE_GRAPHICS_BIT\n");
        }
        if (queuePropArray[idx].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            fprintf(stderr, "VK_QUEUE_TRANSFER_BIT\n");
        }
        if (queuePropArray[idx].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
            fprintf(stderr, "VK_QUEUE_SPARSE_BINDING_BIT\n");
        }
        if (queuePropArray[idx].queueFlags & VK_QUEUE_PROTECTED_BIT) {
            fprintf(stderr, "VK_QUEUE_PROTECTED_BIT\n");
        }
    }
}

int main()
{
    
    constexpr uint32_t defaultDeviceIdx = 0U;
    constexpr uint32_t numRequiredInstanceExtns = 3U;
    constexpr uint32_t numRequiredDeviceExtns = 1U;
    constexpr int32_t width = 1024;
    constexpr int32_t height = 768;
    uint32_t numPhysicalDevices = 0U;
    uint32_t numInstanceExtensions = 0U;
    uint32_t numDeviceExtensions = 0U;
    uint32_t numDeviceQueues = 0U;
    const char *requiredInstanceExtns[] = {
        "VK_KHR_display",
        "VK_KHR_surface",
#ifdef WIN32
        "VK_KHR_win32_surface",
#else
        "VK_KHR_xcb_surface",
#endif
    };
    const char *requiredDeviceExtns[] = {
        "VK_KHR_swapchain"
    };

#ifdef WIN32
    HINSTANCE hInstance = GetModuleHandle(NULL);
    HWND hWindow = CreateWin32Window(width, height);
#endif

    VkResult result;
    VkInstance instance;
    VkDevice device;
    VkSurfaceKHR surface;
    VkSwapchainKHR swapchain;
    VkExtent2D swapchainExtent = {width, height};

    VkPhysicalDevice *physicalDevicesArray;
    VkPhysicalDeviceProperties *physicalDevicePropertiesArray;
    VkExtensionProperties *instanceExtnPropArray;
    VkExtensionProperties *deviceExtnPropArray;
    VkQueueFamilyProperties *queuePropArray;

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.enabledExtensionCount = numRequiredInstanceExtns;
    instanceCreateInfo.ppEnabledExtensionNames = requiredInstanceExtns;

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueCount = 1U;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1U;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = numRequiredDeviceExtns;
    deviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtns;

#ifdef WIN32
    VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfo = {};
    win32SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    win32SurfaceCreateInfo.hinstance = hInstance;
    win32SurfaceCreateInfo.hwnd = hWindow;
#endif

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.minImageCount = 1U;
    swapchainCreateInfo.imageFormat = VK_FORMAT_R8G8B8A8_UINT;
    swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    swapchainCreateInfo.imageExtent = swapchainExtent;
    swapchainCreateInfo.imageArrayLayers = 1U;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;

    if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
        fprintf(stderr, "Failed to Create Vulkan Instance\n");
    } else {
        fprintf(stderr, "Created Vulkan Instance\n");
    }

    if (vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, nullptr) != VK_SUCCESS) {
        fprintf(stderr, "Failed to get the number of physical devices present\n");
    } else {
        fprintf(stderr, "Number of physical devices: %u\n", numPhysicalDevices);
    }

    physicalDevicesArray = new VkPhysicalDevice[numPhysicalDevices];

    if (vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, physicalDevicesArray) != VK_SUCCESS) {
        fprintf(stderr, "Failed to get Physical Device instance\n");
    } else {
        fprintf(stderr, "Got Physical device\n");
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
    } else {
        fprintf(stderr, "Number of Instance Extensions: %u\n", numInstanceExtensions);
    }

    instanceExtnPropArray = new VkExtensionProperties[numInstanceExtensions];

    if (vkEnumerateInstanceExtensionProperties(nullptr, &numInstanceExtensions, instanceExtnPropArray) != VK_SUCCESS) {
        fprintf(stderr, "Failed to query instance extensions\n");
    } else {
        for (uint32_t idx = 0U; idx < numInstanceExtensions; idx++) {
            fprintf(stderr, "%s : %u\n", instanceExtnPropArray[idx].extensionName, instanceExtnPropArray[idx].specVersion);
        }
    }

    if (vkEnumerateDeviceExtensionProperties(physicalDevicesArray[defaultDeviceIdx], nullptr, &numDeviceExtensions, nullptr) != VK_SUCCESS) {
        fprintf(stderr, "Failed to query number of device extensions\n");
    } else {
        fprintf(stderr, "Number of Device Extensions: %u\n", numDeviceExtensions);
    }

    deviceExtnPropArray = new VkExtensionProperties[numDeviceExtensions];

    if (vkEnumerateDeviceExtensionProperties(physicalDevicesArray[defaultDeviceIdx], nullptr, &numDeviceExtensions, deviceExtnPropArray) != VK_SUCCESS) {
        fprintf(stderr, "Failed to query number of device extensions\n");
    } else {
        for (uint32_t idx = 0U; idx < numDeviceExtensions; idx++) {
            fprintf(stderr, "%s : %u\n", deviceExtnPropArray[idx].extensionName, deviceExtnPropArray[idx].specVersion);
        }
    }

    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevicesArray[defaultDeviceIdx], &numDeviceQueues, nullptr);
    fprintf(stderr, "Number of Device Queues: %u\n", numDeviceQueues);

    queuePropArray = new VkQueueFamilyProperties[numDeviceQueues];
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevicesArray[defaultDeviceIdx], &numDeviceQueues, queuePropArray);
     
    ParseDeviceQueueDetails(numDeviceQueues, queuePropArray);

    if (vkCreateDevice(physicalDevicesArray[defaultDeviceIdx], &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create VkDevice\n");
    } else {
        fprintf(stderr, "Created VkDevice\n");
    }

#ifdef WIN32
    if (vkCreateWin32SurfaceKHR(instance, &win32SurfaceCreateInfo, nullptr, &surface) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create VkSurface\n");
    } else {
        fprintf(stderr, "Created Win32 VkSurface\n");
    }
#endif

    swapchainCreateInfo.surface = surface;

    if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create VkSwapchain\n");
    } else {
        fprintf(stderr, "Created VkSwapchain\n");
    }

    delete[] physicalDevicesArray;
    delete[] physicalDevicePropertiesArray;
    delete[] instanceExtnPropArray;
    delete[] deviceExtnPropArray;
    delete[] queuePropArray;
}