#include <iostream>
#include <vector>

#ifdef _WIN32
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#else
#define VK_USE_PLATFORM_XCB_KHR
#endif
#include <vulkan/vulkan.h>

#include "../include/GLPGWindow.hpp"
#include "../include/GLPGEvent.hpp"
#include "../include/utils/vk-print.hpp"
#include "../include/utils/vk-utils.hpp"
#include <random>
#include <fstream>

#undef CreateWindow

const float vertexData[] = {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
};

template <typename T>
class counter {
public:
    counter(T val) : m_val(val), performAddition(true) {};
    counter() : m_val(T(0)), performAddition(true) {};

    // Perhaps the most stupidest op overload ever.
    T operator++() {
        if (performAddition) {
            m_val += T(0.001);
        } else {
            m_val -= T(0.001);
        }

        if (m_val > T(1)|| m_val <= T(0)) {
            performAddition = !performAddition;
        }
        return m_val;
    }

private:
    T m_val;
    bool performAddition;
};

using counter_f = counter<float>;
int main()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0.0, 1.0);
    counter_f red(dist(gen));
    counter_f green(dist(gen));
    counter_f blue(dist(gen));
    const char *compiledVSPath = "C:/repos/GLPG/vulkan/shaders/a.spv";
    const char *vsCompiledBlob;
    const char *compiledFSPath = "C:/repos/GLPG/vulkan/shaders/a_fs.spv";
    const char *fsCompiledBlob;
    ///
    /// TODO: Query modes dynamically.
    ///
    constexpr uint32_t width = 2560U;
    constexpr uint32_t height = 1440U;
    GLPG::GLPGEventLoop eventLoop;
    GLPG::GLPGEvent event;

    std::fstream fs;
    std::streampos vsCompiledBlobSize;
    std::streampos fsCompiledBlobSize;

    fs.open(compiledVSPath, std::ios::in | std::ios::binary | std::ios::ate);
    if (!fs.is_open()) {
        std::cerr << "Failed to open spv file\n";
        return -1;
    } else {
        std::cerr << "Opened spv file\n";
    }

    vsCompiledBlobSize = fs.tellg();
    vsCompiledBlob = new char[vsCompiledBlobSize];
    fs.seekg(0, std::ios::beg);
    fs.read((char *)(vsCompiledBlob), vsCompiledBlobSize);
    fs.close();

    fs.open(compiledFSPath, std::ios::in | std::ios::binary | std::ios::ate);
    if (!fs.is_open()) {
        std::cerr << "Failed to open spv file\n";
        return -1;
    } else {
        std::cerr << "Opened spv file\n";
    }

    fsCompiledBlobSize = fs.tellg();
    fsCompiledBlob = new char[fsCompiledBlobSize];
    fs.seekg(0, std::ios::beg);
    fs.read((char *)(fsCompiledBlob), fsCompiledBlobSize);
    fs.close();

    GLPG::GLPGWindow *window = GLPG::GLPGWindow::GetInstance();
    if (!window) {
        std::cerr << "Failed to get GLPGWindow instance\n";
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

    std::vector<const char*> requiredInstanceExtensions = {"VK_KHR_surface"};
#ifdef _WIN32
    requiredInstanceExtensions.emplace_back("VK_KHR_win32_surface");
#endif // _WIN32
#ifdef GLPG_IS_DEBUG
    requiredInstanceExtensions.emplace_back("VK_EXT_debug_utils");
#endif // GLPG_IS_DEBUG
    std::vector<const char*> requiredInstanceLayers = {};
#ifdef GLPG_IS_DEBUG
    requiredInstanceLayers.emplace_back("VK_LAYER_KHRONOS_validation");
#endif // GLPG_IS_DEBUG

    ///
    /// Query the number of available instance layers.
    ///
    uint32_t n_availableInstanceLayers = 0;
    VkResult res = vkEnumerateInstanceLayerProperties(&n_availableInstanceLayers,
                                                      nullptr);
    if (res == VK_SUCCESS) {
        std::cout << "No. Total Instance Layers Available: " << \
            n_availableInstanceLayers << "\n";
    } else {
        std::cerr << "Failed to query the number of available instance layers\n";
        return -1;
    }

    ///
    /// Query the properties of all available instance layers.
    ///
    std::vector<VkLayerProperties> availableInstanceLayers(n_availableInstanceLayers);
    res = vkEnumerateInstanceLayerProperties(&n_availableInstanceLayers,
                                             availableInstanceLayers.data());
    if (res == VK_SUCCESS) {
        std::cout << "Available Instance Layers: \n";
        for (auto prop : availableInstanceLayers) {
            std::cout << "\t " << prop.layerName << "\n";
            std::cout << "\t\t " << prop.description << "\n";
        }
    } else {
        std::cerr << "Failed to query Instance Layer Properties\n";
        return -1;
    }

    ///
    /// Query the number of available default instance extensions.
    ///
    uint32_t n_availableDefaultInstanceExtensions = 0;
    res = vkEnumerateInstanceExtensionProperties(nullptr,
                                                 &n_availableDefaultInstanceExtensions,
                                                 nullptr);
    if (res == VK_SUCCESS) {
        std::cout << "No. Total Default Instance Extensions Available: " << \
            n_availableDefaultInstanceExtensions << "\n";
    } else {
        std::cerr << "Failed to query the number of available default instance extensions\n";
        return -1;
    }

    ///
    /// Query the properties of all available default instance extensions.
    ///
    std::vector<VkExtensionProperties> availableInstanceExtensions(n_availableDefaultInstanceExtensions);
    res = vkEnumerateInstanceExtensionProperties(nullptr,
                                                 &n_availableDefaultInstanceExtensions,
                                                 availableInstanceExtensions.data());
    if (res == VK_SUCCESS) {
        std::cout << "Available Default Instance Extensions: \n";
        for (auto prop : availableInstanceExtensions) {
            std::cout << "\t " << prop.extensionName << "\n";
        }
    } else {
        std::cerr << "Failed to query Default Instance Extension Properties\n";
        return -1;
    }

    ///
    /// Query the number of available layer instance extensions for each discovered layer.
    ///
    std::vector<uint32_t> n_availableLayerInstanceExtensions(n_availableInstanceLayers);
    uint32_t idx = 0U;
    for (auto prop : availableInstanceLayers) {
        res = vkEnumerateInstanceExtensionProperties(prop.layerName,
                                                     &n_availableLayerInstanceExtensions[idx],
                                                     nullptr);
        if (res == VK_SUCCESS) {
            std::cout << "No. Total Layer " << prop.layerName <<\
                " Instance Extensions Available: " <<\
                n_availableLayerInstanceExtensions[idx] << "\n";
            if (n_availableLayerInstanceExtensions[idx] > 0) {
                ///
                /// Query the properties of available layer instance extensions.
                ///
                std::vector<VkExtensionProperties> layerInstanceExtensions(n_availableLayerInstanceExtensions[idx]);
                res = vkEnumerateInstanceExtensionProperties(prop.layerName,
                                                             &n_availableLayerInstanceExtensions[idx],
                                                             layerInstanceExtensions.data());
                if (res == VK_SUCCESS) {
                    for (auto layerProp : layerInstanceExtensions) {
                        std::cout << "\t " << layerProp.extensionName << "\n";
                    }
                }
            }
        } else {
            std::cerr << "Failed to query extensions for " << prop.layerName << " layer\n";
            return -1;
        }
        idx++;
    }

    ///
    /// Create a VkInstance with the required list of instance layers and extension.
    ///
    /// The following instance extensions are currently enabled:
    /// 1. VK_KHR_surface
    /// 2. VK_KHR_win32_surface
    /// 3. VK_EXT_debug_utils (Only on debug builds)
    ///
    /// The following instance layers are currently enabled:
    /// 1. VK_LAYER_KHRONOS_validation
    ///
    VkInstance instance = {};
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.enabledLayerCount = static_cast<uint32_t>(requiredInstanceLayers.size());
    createInfo.ppEnabledLayerNames = requiredInstanceLayers.data();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredInstanceExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredInstanceExtensions.data();

    res = vkCreateInstance(&createInfo, nullptr, &instance);
    if (res != VK_SUCCESS) {
        std::cerr << "vkCreateInstance failed\n";
        return -1;
    }

    ///
    /// Query the number of available physical devices.
    ///
    uint32_t numPhysicalDevs = 0U;
    res = vkEnumeratePhysicalDevices(instance, &numPhysicalDevs, nullptr);
    if (res != VK_SUCCESS || numPhysicalDevs <= 0U) {
        std::cerr << "Failed to query physical devices\n";
    } else {
        // We default to using the first enumerated VkPhysicalDevice.
        numPhysicalDevs = 1U;
    }

    ///
    /// Query the handle for the first VkPhysicalDevice
    ///
    VkPhysicalDevice physicalDev = {};
    res = vkEnumeratePhysicalDevices(instance, &numPhysicalDevs, &physicalDev);
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to query the first physical device\n";
        return -1;
    }

    ///
    /// Query properties of the physical device
    ///
    VkPhysicalDeviceProperties physicalDevProps = {};
    vkGetPhysicalDeviceProperties(physicalDev, &physicalDevProps);
    std::cout << "Using Device:\n";
    std::cout << "\t Name: " << physicalDevProps.deviceName << "\n";
    std::cout << "\t Vulkan API: " << VK_VERSION_MAJOR(physicalDevProps.apiVersion) \
                                   << "." << VK_VERSION_MINOR(physicalDevProps.apiVersion) \
                                   << "." << VK_VERSION_PATCH(physicalDevProps.apiVersion) << "\n";

    ///
    /// Query the number of available device extensions
    ///
    uint32_t n_availableDeviceExtensions = 0U;
    res = vkEnumerateDeviceExtensionProperties(physicalDev, nullptr, &n_availableDeviceExtensions,
                                               nullptr);
    if (res == VK_SUCCESS) {
        std::cout << "No. Total Device Extensions: " << n_availableDeviceExtensions << "\n";
    } else {
        std::cerr << "Failed to query the number of Device Extensions\n";
        return -1;
    }

    std::vector<VkExtensionProperties> availableDeviceExtensions(n_availableDeviceExtensions);
    res = vkEnumerateDeviceExtensionProperties(physicalDev, nullptr, &n_availableDeviceExtensions,
                                               availableDeviceExtensions.data());
    if (res == VK_SUCCESS) {
        std::cout << "Available Device Extensions:\n";
        for (auto prop : availableDeviceExtensions) {
            std::cout << "\t " << prop.extensionName << " " << prop.specVersion << "\n";
        }
    }

    std::vector<const char*> requiredDeviceExtensions = {"VK_KHR_swapchain"};

    ///
    /// Query the number of available queues with the Vulkan Device.
    ///
    uint32_t n_queueCount = 0U;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDev, &n_queueCount, nullptr);
    std::cout << "No. Total Queues: " << n_queueCount << "\n";

    std::vector<VkQueueFamilyProperties> availableQueueProps(n_queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDev, &n_queueCount, availableQueueProps.data());
    std::cout << "Queue Properties: " << "\n";
    ParseDeviceQueueDetails(n_queueCount, availableQueueProps.data());


    ///
    /// On Nvidia, the first enumerated queue family supports every o/p. Hardcode for now to use it.
    /// TODO: This might not work on AMD/Intel. Fix.
    ///
    constexpr float queuePriority = 1.0F;
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = 0U;
    queueCreateInfo.queueCount = 1U;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo devCreateInfo = {};
    devCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    devCreateInfo.queueCreateInfoCount = 1U;
    devCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    devCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
    devCreateInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

    VkDevice dev = {};
    res = vkCreateDevice(physicalDev, &devCreateInfo, nullptr, &dev);
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to create a VkDevice\n";
    }

    VkQueue queue = {};
    vkGetDeviceQueue(dev, 0, 0, &queue);

    ///
    /// Query the available memory heaps and types.
    ///
    VkPhysicalDeviceMemoryProperties physcalDevMemoryProps = {};
    vkGetPhysicalDeviceMemoryProperties(physicalDev, &physcalDevMemoryProps);
    VkPrintVkPhysicalDeviceMemoryProperties(physcalDevMemoryProps);

    ///
    /// Create Shader modules
    ///
    VkShaderModuleCreateInfo vsShaderModuleCreateInfo = {};
    vsShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    vsShaderModuleCreateInfo.codeSize = vsCompiledBlobSize;
    vsShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t *>(vsCompiledBlob);
    VkShaderModule vsShaderModule = {};

    res = vkCreateShaderModule(dev, &vsShaderModuleCreateInfo, nullptr, &vsShaderModule);
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to create Vertex shader module\n";
        return -1;
    }

    VkShaderModuleCreateInfo fsShaderModuleCreateInfo = {};
    fsShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    fsShaderModuleCreateInfo.codeSize = fsCompiledBlobSize;
    fsShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t *>(fsCompiledBlob);
    VkShaderModule fsShaderModule = {};

    res = vkCreateShaderModule(dev, &fsShaderModuleCreateInfo, nullptr, &fsShaderModule);
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to create Fragment shader module\n";
        return -1;
    }

    ///
    /// Create Win32 VkSurface
    ///
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = GetModuleHandle(NULL);
    surfaceCreateInfo.hwnd = *(reinterpret_cast<HWND *>(window->GetWindowHandle()));

    VkSurfaceKHR surface = {};
    res = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to create VkSurfaceKHR for Window\n";
        return -1;
    }

    VkBool32  surfacePresentSupported = VK_FALSE;
    res = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDev, 0, surface, &surfacePresentSupported);
    if (res != VK_SUCCESS) {
        std::cerr << "Queue does not support presenting to the created VkSurface\n";
        return -1;
    }

    ///
    /// Query the caps of the surface.
    ///
    VkSurfaceCapabilitiesKHR surfaceCaps = {};
    res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDev, surface, &surfaceCaps);
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to query surface caps\n";
        return -1;
    } else {
         VkPrintVkSurfaceCapabilitiesKHR(surfaceCaps);
    }


    ///
    /// Create a swapchain.
    ///
    uint32_t n_supportedSurfaceFormats = 0U;
    bool foundSupportedFormat = false;
    uint32_t formatIdx = 0U;
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDev, surface, &n_supportedSurfaceFormats, nullptr);
    if (res == VK_SUCCESS) {
        std::cout << "Number of formats supported: " << n_supportedSurfaceFormats << "\n";
    } else {
        std::cerr << "Failed to query the number of available surface formats\n";
        return -1;
    }

    std::vector<VkSurfaceFormatKHR> surfaceFormats(n_supportedSurfaceFormats);
    res = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDev, surface, &n_supportedSurfaceFormats, surfaceFormats.data());
    if (res == VK_SUCCESS) {
        uint32_t idx = 0U;
        std::cout << "Supported Surface Formats: " << "\n";
        for (auto format : surfaceFormats) {
            VkPrintVkSurfaceFormatKHR(format);
            if (format.format == VK_FORMAT_B8G8R8A8_UNORM) {
                foundSupportedFormat = true;
                formatIdx = idx;
            }
            idx++;
        }
    } else {
        std::cerr << "Failed to query the available surface formats\n";
        return -1;
    }

    if (!foundSupportedFormat) {
        std::cerr << "VkSurface does not support VK_FORMAT_B8G8R8A8_UNORM. Exiting\n";
        return -1;
    }
    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = 2U;
    swapchainCreateInfo.imageFormat = surfaceFormats[formatIdx].format;
    swapchainCreateInfo.imageColorSpace = surfaceFormats[formatIdx].colorSpace;
    swapchainCreateInfo.imageExtent = surfaceCaps.currentExtent;
    swapchainCreateInfo.imageArrayLayers = 1U;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;

    VkSwapchainKHR swapchain = {};
    res = vkCreateSwapchainKHR(dev, &swapchainCreateInfo, nullptr, &swapchain);
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to create swapchain\n";
        return -1;
    }

    ///
    /// Query the swapchain images
    ///
    uint32_t n_swapchainImages = 0U;
    res = vkGetSwapchainImagesKHR(dev, swapchain, &n_swapchainImages, nullptr);
    if (res == VK_SUCCESS) {
        std::cout << "Number of swapchain image: " << n_swapchainImages << "\n";
    } else {
        std::cerr << "Failed to query the number of swapchain images\n";
        return -1;
    } 

    std::vector<VkImage> swapchainImages(n_swapchainImages);
    res = vkGetSwapchainImagesKHR(dev, swapchain, &n_swapchainImages, swapchainImages.data());
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to query swapchain images\n";
        return -1;
    }

    ///
    /// Create Command Pool.
    ///
    VkCommandPool cmdPool = {};
    VkCommandPoolCreateInfo cmdPoolCreateInfo = {};
    cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    res = vkCreateCommandPool(dev, &cmdPoolCreateInfo, nullptr, &cmdPool);
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to create Command pool\n";
        return -1;
    }

    ///
    /// Allocate command buffers from command pool.
    ///
    VkCommandBuffer cmdBuffer = {};
    VkCommandBufferAllocateInfo cmdBufferAllocInfo = {};
    cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufferAllocInfo.commandPool = cmdPool;
    cmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufferAllocInfo.commandBufferCount = 1U;
    res = vkAllocateCommandBuffers(dev, &cmdBufferAllocInfo, &cmdBuffer);
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to create command buffer\n";
        return -1;
    }

    ///
    /// Create a fence to be signaled by the device when swapchain images are available.
    ///
    VkFence imageAcquireFence = {};
    VkFenceCreateInfo imageAcquireFenceCreateInfo = {};
    imageAcquireFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    res = vkCreateFence(dev, &imageAcquireFenceCreateInfo, nullptr, &imageAcquireFence);
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to create vk fence\n";
        return -1;
    } else {
        std::cout << "Created vk fence\n";
    }

    ///
    /// Create a fence to be signaled by the device when the submitted work finishes.
    ///
    VkFence queueSubmitPostFence = {};
    VkFenceCreateInfo queueSubmitPostFenceCreateInfo = {};
    queueSubmitPostFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    queueSubmitPostFenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    res = vkCreateFence(dev, &queueSubmitPostFenceCreateInfo, nullptr, &queueSubmitPostFence);
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to create vk fence\n";
        return -1;
    } else {
        std::cout << "Created vk fence\n";
    }


    ///
    /// Renderpass creation
    ///

    ///
    /// First, describe the sole swapchain attachment.
    /// We have two attachment descriptions here since our
    /// swapchain contains two color buffers.
    ///
    std::vector<VkAttachmentDescription> swapchainAttachmentDescs;
    VkAttachmentDescription swapchainAttachmentDesc = {};
    swapchainAttachmentDesc.format = VK_FORMAT_B8G8R8A8_UNORM;
    swapchainAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
    swapchainAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    swapchainAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    swapchainAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    swapchainAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    swapchainAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    swapchainAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; 
    swapchainAttachmentDescs.emplace_back(swapchainAttachmentDesc);
    swapchainAttachmentDescs.emplace_back(swapchainAttachmentDesc);

    ///
    /// Next, describe the sole subpass inside the renderpass.
    ///

    VkAttachmentReference swapchainImageReference = {};
    swapchainImageReference.attachment = 0U;
    swapchainImageReference.layout = VK_IMAGE_LAYOUT_GENERAL;

    VkSubpassDescription colorSubpassDesc = {};
    colorSubpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    colorSubpassDesc.colorAttachmentCount = 1U;
    colorSubpassDesc.pColorAttachments = &swapchainImageReference;

    ///
    /// Finally, describe and create the render pass
    ///
    VkRenderPassCreateInfo clearRenderPassCreateInfo = {};
    clearRenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    clearRenderPassCreateInfo.attachmentCount = 1U;
    clearRenderPassCreateInfo.pAttachments = &swapchainAttachmentDescs[0];
    clearRenderPassCreateInfo.subpassCount = 1U;
    clearRenderPassCreateInfo.pSubpasses = &colorSubpassDesc;

    VkRenderPass clearRenderPass = {};
    res = vkCreateRenderPass(dev, &clearRenderPassCreateInfo, nullptr, &clearRenderPass);
    if (res != VK_SUCCESS) {
        std::cout << "Failed to create renderpass\n";
        return -1;
    } else {
        std::cout << "Created Renderpass\n";
    }

    ///
    /// Framebuffer creation.
    ///

    ///
    /// First, create VkImageView handles for the swapchain images.
    ///
    std::vector<VkImageView> swapchainImageViews(2U);
    for (uint32_t idx = 0U; idx < 2U; idx++) {
        VkImageViewCreateInfo swapchainImageViewInfo = {};
        swapchainImageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        swapchainImageViewInfo.image = swapchainImages[idx];
        swapchainImageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        swapchainImageViewInfo.format = VK_FORMAT_B8G8R8A8_UNORM;
        swapchainImageViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        swapchainImageViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        swapchainImageViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        swapchainImageViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        swapchainImageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        swapchainImageViewInfo.subresourceRange.levelCount = 1;
        swapchainImageViewInfo.subresourceRange.layerCount = 1;

        res = vkCreateImageView(dev, &swapchainImageViewInfo, nullptr, &swapchainImageViews[idx]);
        if (res != VK_SUCCESS) {
            std::cout << "Failed to create image views for swapchain image idx: " << idx << "\n";
            return -1;
        }
    }

    ///
    /// Next, create a framebuffer object encompassing the swapchain image views.
    ///
    std::vector<VkFramebuffer> fb;
    for (uint32_t idx = 0U; idx < 2U; idx++) {
        VkFramebufferCreateInfo fbCreateInfo = {};
        fbCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbCreateInfo.renderPass = clearRenderPass;
        fbCreateInfo.attachmentCount = 1U; // number of swapchain images.
        fbCreateInfo.pAttachments = &swapchainImageViews[idx];
        fbCreateInfo.width = surfaceCaps.currentExtent.width;
        fbCreateInfo.height = surfaceCaps.currentExtent.height;
        fbCreateInfo.layers = 1;
        VkFramebuffer swapchainFb = {};
        res = vkCreateFramebuffer(dev, &fbCreateInfo, nullptr, &swapchainFb);
        if (res != VK_SUCCESS) {
            std::cout << "Failed to create Framebuffer\n";
            return -1;
        } else {
            std::cout << "Created framebuffer for swapchain image: " << idx << "\n";
        }
        fb.emplace_back(swapchainFb);
    }

    ///
    /// Create Shader resources.
    ///

    ///
    /// First, create a VkBuffer object for storing the vertex data.
    /// Currently we only have position information as part of our vertex data.
    ///
    VkBuffer vtxBuffer = {};
    VkBufferCreateInfo vtxInputBufferCreateInfo = {};
    vtxInputBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vtxInputBufferCreateInfo.size = 9 * sizeof(float);
    vtxInputBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    std::cout << "Req size: " << vtxInputBufferCreateInfo.size << "\n";

    res = vkCreateBuffer(dev, &vtxInputBufferCreateInfo, nullptr, &vtxBuffer);
    if (res != VK_SUCCESS) {
        std::cout << "Failed to create input vtx buffer\n";
        return -1;
    }

    ///
    /// Get the memory requirements for the created VkBuffer object
    ///
    VkMemoryRequirements vtxInputBufferReqs = {};
    vkGetBufferMemoryRequirements(dev, vtxBuffer, &vtxInputBufferReqs);
    PrintVkMemoryRequirements(vtxInputBufferReqs);

    
    VkMemoryAllocateInfo vtxInputBufferMemInfo = {};
    vtxInputBufferMemInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    vtxInputBufferMemInfo.allocationSize = vtxInputBufferReqs.size;
    vtxInputBufferMemInfo.memoryTypeIndex = 3U; // Use the 256 MiB sized BAR mapping on Intel Xe Alchemist.
    VkDeviceMemory devMemory = {};

    ///
    /// Allocate device memory backing for the VkBuffer object.
    ///
    res = vkAllocateMemory(dev, &vtxInputBufferMemInfo, nullptr, &devMemory);
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to allocate memory\n";
        return -1;
    }

    ///
    /// Map the allocated device memory into the CPU's VA
    /// and copy the vertex data into the memory before unmapping it.
    ///
    float *cpuVAPtr = nullptr;
    res = vkMapMemory(dev, devMemory, 0U, VK_WHOLE_SIZE, 0U, reinterpret_cast<void **>(&cpuVAPtr));
    if (res != VK_SUCCESS || cpuVAPtr == nullptr) {
        std::cerr << "Failed to map memory\n";
        return -1;
    } else {
        std::cout << "VA PTR: " << cpuVAPtr << "\n";
        std::cout << "Allocated size: " << vtxInputBufferReqs.size << "\n";
        std::cout << "Input size: " << sizeof(vertexData) << "\n";
    }

    /*
    float *tmpPtr = cpuVAPtr;
    for (uint32_t idx = 0U; idx < sizeof(vertexData) / sizeof(float); idx++) {
        *tmpPtr++ = vertexData[idx];
    }*/

    std::memcpy(cpuVAPtr, vertexData, sizeof(vertexData));

    vkUnmapMemory(dev, devMemory);

    ///
    /// Bind the allocated vkDeviceMemory with the vkBuffer
    ///
    res = vkBindBufferMemory(dev, vtxBuffer, devMemory, 0U);
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to bind VkDeviceMemory to VkBuffer\n";
        return -1;
    }

    VkDescriptorSetLayoutBinding defaultBindingLayout = {};
    defaultBindingLayout.binding = 0U;
    defaultBindingLayout.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    defaultBindingLayout.descriptorCount = 1U;
    defaultBindingLayout.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayout defaultDescriptorSetLayout = {};
    VkDescriptorSetLayoutCreateInfo descSetLayoutCreateInfo = {};
    descSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descSetLayoutCreateInfo.bindingCount = 1U;
    descSetLayoutCreateInfo.pBindings = &defaultBindingLayout;

    res = vkCreateDescriptorSetLayout(dev, &descSetLayoutCreateInfo, nullptr, &defaultDescriptorSetLayout);
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to create descriptor set\n";
        return -1;
    } else {
        std::cerr << "Created descriptor set layout\n";
    }

    VkPipelineLayout defaultPipelineLayout = {};
    VkPipelineLayoutCreateInfo defaultPipelineLayoutCreateInfo = {};
    defaultPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    defaultPipelineLayoutCreateInfo.setLayoutCount = 1U;
    defaultPipelineLayoutCreateInfo.pSetLayouts = &defaultDescriptorSetLayout;

    res = vkCreatePipelineLayout(dev, &defaultPipelineLayoutCreateInfo, nullptr, &defaultPipelineLayout);
    if (res != VK_SUCCESS) {
        std::cout << "Failed to create pipeline layout\n";
        return -1;
    }

    ///
    /// Create the sole PSO.
    ///
    std::vector<VkPipelineShaderStageCreateInfo> shaderStagesCreateInfo = {};
    VkPipelineShaderStageCreateInfo vtxStageCreateInfo = {};
    vtxStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vtxStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vtxStageCreateInfo.module = vsShaderModule;
    vtxStageCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragStageCreateInfo = {};
    fragStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStageCreateInfo.module = fsShaderModule;
    fragStageCreateInfo.pName = "main";

    shaderStagesCreateInfo.emplace_back(vtxStageCreateInfo);
    shaderStagesCreateInfo.emplace_back(fragStageCreateInfo);

    VkVertexInputBindingDescription vtxInputBindingDesc = {};
    vtxInputBindingDesc.binding = 0U;
    vtxInputBindingDesc.stride = sizeof(float);
    vtxInputBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription vtxInputAttributeDesc = {};
    vtxInputAttributeDesc.location = 0U;
    vtxInputAttributeDesc.binding = 0U;
    vtxInputAttributeDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
    vtxInputAttributeDesc.offset = 0U;

    VkPipelineVertexInputStateCreateInfo vtxInputStateCreateInfo = {};
    vtxInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vtxInputStateCreateInfo.vertexBindingDescriptionCount = 1U;
    vtxInputStateCreateInfo.pVertexBindingDescriptions = &vtxInputBindingDesc;
    vtxInputStateCreateInfo.vertexAttributeDescriptionCount = 1U;
    vtxInputStateCreateInfo.pVertexAttributeDescriptions = &vtxInputAttributeDesc;

    VkPipelineInputAssemblyStateCreateInfo IAStateCreateInfo = {};
    IAStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    IAStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkViewport viewport = {};
    viewport.x = 0.0F;
    viewport.y = 0.0F;
    viewport.width = static_cast<float>(surfaceCaps.currentExtent.width);
    viewport.height = static_cast<float>(surfaceCaps.currentExtent.height);
    viewport.minDepth = 0.0F;
    viewport.maxDepth = 1.0F;

    VkRect2D scissor = {};
    scissor.extent.width = surfaceCaps.currentExtent.width;
    scissor.extent.height = surfaceCaps.currentExtent.height;

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1U;
    viewportStateCreateInfo.pViewports = &viewport;
    viewportStateCreateInfo.scissorCount = 1U;
    viewportStateCreateInfo.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.lineWidth = 1.0F;

    VkPipelineMultisampleStateCreateInfo msStateCreateInfo = {};
    msStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    msStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
    colorBlendAttachmentState.blendEnable = false;
    colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
    colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.attachmentCount = 1U;
    colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
    
    VkGraphicsPipelineCreateInfo graphicsPSOCreateInfo = {};
    graphicsPSOCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPSOCreateInfo.stageCount = 2U; // Vertex and Fragment stages.
    graphicsPSOCreateInfo.pStages = shaderStagesCreateInfo.data();
    graphicsPSOCreateInfo.pVertexInputState = &vtxInputStateCreateInfo;
    graphicsPSOCreateInfo.pInputAssemblyState = &IAStateCreateInfo;
    graphicsPSOCreateInfo.pViewportState = &viewportStateCreateInfo;
    graphicsPSOCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    graphicsPSOCreateInfo.pMultisampleState = &msStateCreateInfo;
    graphicsPSOCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    graphicsPSOCreateInfo.layout = defaultPipelineLayout;
    graphicsPSOCreateInfo.renderPass = clearRenderPass; 
    graphicsPSOCreateInfo.subpass = 0U;

    VkPipeline pipeline = {};
    res = vkCreateGraphicsPipelines(dev, VK_NULL_HANDLE, 1U, &graphicsPSOCreateInfo, nullptr, &pipeline);
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to create graphics PSO\n";
        return -1;
    } else {
        std::cout << "Created graphics pipeline\n";
    }

    while ((event = eventLoop.GetEvent()) != GLPG::GLPGEvent::Key_Escape) {
        ///
        /// Reset the image acquire fence.
        ///
        res = vkResetFences(dev, 1U, &imageAcquireFence);
        if (res != VK_SUCCESS) {
            std::cerr << "Failed to reset fence\n";
        }

        ///
        /// Query the status of the post-submit fence.
        /// Wait on the fence if the fence is not signaled
        /// and reset the fence.
        ///
        res = vkGetFenceStatus(dev, queueSubmitPostFence);
        if (res == VK_ERROR_DEVICE_LOST) {
            std::cerr << "Failed to get post submit fence status\n";
            return -1;
        } else if (res == VK_SUCCESS) {
            res = vkResetFences(dev, 1U, &queueSubmitPostFence);
            if (res != VK_SUCCESS) {
                std::cerr << "Failed to reset post submit fence\n";
                return -1;
            }
        } else {
            res = vkWaitForFences(dev, 1U, &queueSubmitPostFence, VK_TRUE, 1000000000U);
            if (res != VK_SUCCESS) {
                std::cerr << "Timed out waiting for post submit fence\n";
                return -1;
            }
            res = vkResetFences(dev, 1U, &queueSubmitPostFence);
            if (res != VK_SUCCESS) {
                std::cerr << "Failed to reset post submit fence TAG B\n";
                return -1;
            }
        }

        ///
        /// Acquire the next available swapchain image from the swapchain.
        ///
        uint32_t acquiredImageIdx = 0U;
        res = vkAcquireNextImageKHR(dev, swapchain, 100000000U, VK_NULL_HANDLE,
                                    imageAcquireFence, &acquiredImageIdx);
        if (res != VK_SUCCESS) {
            std::cerr << "Failed to acquire swapchain image\n";
            std::cerr << "Err: " << res << "\n";
            return -1;
        }

        ///
        /// Wait on the acquire fence.
        ///
        res = vkWaitForFences(dev, 1U, &imageAcquireFence, VK_TRUE, 100000000U);
        if (res != VK_SUCCESS) {
            std::cerr << "Timed out waiting for image acquisition\n";
            return -1;
        }
 
        VkClearColorValue color = {0.0, 0.0, 190.0, 0.0};
        
        VkImageSubresourceRange range = {};
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        range.levelCount = 1;
        range.layerCount = 1;

        ///
        /// Begin command buffer recording.
        ///
        VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
        cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        res = vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);
        if (res != VK_SUCCESS) {
            std::cerr << "Failed to begin command buffer\n";
            return -1;
        }

        vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

        uint64_t offset = 0U;
        vkCmdBindVertexBuffers(cmdBuffer, 0U, 1U, &vtxBuffer, &offset);

        ///
        /// Begin the renderpass
        ///
        VkClearValue clearValues[2];
        clearValues[0].color = clearValues[1].color = color;

        VkRect2D renderArea = {};
        renderArea.extent = surfaceCaps.currentExtent;

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = clearRenderPass;
        renderPassBeginInfo.framebuffer = fb[acquiredImageIdx]; 
        renderPassBeginInfo.renderArea = renderArea;
        renderPassBeginInfo.clearValueCount = 2U;
        renderPassBeginInfo.pClearValues = clearValues;

        vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        ///
        /// Insert the command to draw a triangle
        ///
        vkCmdDraw(cmdBuffer, 3, 1, 0, 0);

        ///
        /// End the renderpass
        ///
        vkCmdEndRenderPass(cmdBuffer);

        ///
        /// End command buffer recording.
        ///
        res = vkEndCommandBuffer(cmdBuffer);
        if (res != VK_SUCCESS) {
            std::cerr << "Failed to end command buffer\n";
            return -1;
        }

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1U;
        submitInfo.pCommandBuffers = &cmdBuffer;

        ///
        /// Submit the recorded command buffer to the GPU for rendering.
        ///
        res = vkQueueSubmit(queue, 1U, &submitInfo, queueSubmitPostFence);
        if (res != VK_SUCCESS) {
            std::cerr << "Failed to kickoff\n";
            return -1;
        }

        ///
        /// Wait on the post-submit fence.
        ///
        res = vkWaitForFences(dev, 1U, &queueSubmitPostFence, VK_TRUE, 1000000000U);
        if (res != VK_SUCCESS) {
            std::cerr << "Failed to wait on post-flip fence\n";
            return -1;
        }

        ///
        /// Present the swapchain image.
        ///        
        VkPresentInfoKHR presentInfo = {};
        VkResult presentResult = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.swapchainCount = 1U;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.pImageIndices = &acquiredImageIdx;
        presentInfo.pResults = &presentResult;
        res = vkQueuePresentKHR(queue, &presentInfo);
        if (res != VK_SUCCESS) {
            std::cerr << "Failed to present\n";
            return -1;
        } else if (presentResult != VK_SUCCESS) {
            std::cerr << "Swap failed\n";
            return -1;
        }
    }
}
