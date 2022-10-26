#include <iostream>
#include <vector>

#ifdef _WIN32
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

#undef CreateWindow

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
    ///
    /// TODO: Query modes dynamically.
    ///
    constexpr uint32_t width = 2560U;
    constexpr uint32_t height = 1440U;
    GLPG::GLPGEventLoop eventLoop;
    GLPG::GLPGEvent event;

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
#ifdef VKBUBBLE_DEBUG
    requiredInstanceExtensions.emplace_back("VK_EXT_debug_utils");
#endif // VKBUBBLE_DEBUG
    std::vector<const char*> requiredInstanceLayers = {};
#ifdef VKBUBBLE_DEBUG
    requiredInstanceLayers.emplace_back("VK_LAYER_KHRONOS_validation");
#endif // VKBUBBLE_DEBUG

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

    uint32_t numPhysicalDevs = 0U;
    res = vkEnumeratePhysicalDevices(instance, &numPhysicalDevs, nullptr);
    if (res != VK_SUCCESS || numPhysicalDevs <= 0U) {
        std::cerr << "Failed to query physical devices\n";
    } else {
        // We default to using the first enumerated VkPhysicalDevice.
        numPhysicalDevs = 1U;
    }

    VkPhysicalDevice physicalDev = {};
    res = vkEnumeratePhysicalDevices(instance, &numPhysicalDevs, &physicalDev);
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to query the first physical device\n";
        return -1;
    }

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
    /// Create Command Pool and Command Buffer.
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

    VkSemaphore imageAcquireSemaphore = {};
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    res = vkCreateSemaphore(dev, &semaphoreCreateInfo, nullptr, &imageAcquireSemaphore);
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to create semaphore object\n";
        return -1;
    } else {
        std::cout << "Created VkSemaphore\n";
    }

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
    while ((event = eventLoop.GetEvent()) != GLPG::GLPGEvent::Key_Escape) {
        res = vkResetFences(dev, 1U, &imageAcquireFence);
        if (res != VK_SUCCESS) {
            std::cerr << "Failed to reset fence\n";
        }

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


        uint32_t acquiredImageIdx = 0U;
        res = vkAcquireNextImageKHR(dev, swapchain, 100000000U, VK_NULL_HANDLE,
                                    imageAcquireFence, &acquiredImageIdx);
        if (res != VK_SUCCESS) {
            std::cerr << "Failed to acquire swapchain image\n";
            std::cerr << "Err: " << res << "\n";
            return -1;
        }

        res = vkWaitForFences(dev, 1U, &imageAcquireFence, VK_TRUE, 100000000U);
        if (res != VK_SUCCESS) {
            std::cerr << "Timed out waiting for image acquisition\n";
            return -1;
        }
 
        VkImageSubresourceRange acquireRange = {};
        acquireRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        acquireRange.levelCount = 1;
        acquireRange.layerCount = 1;
        VkImageMemoryBarrier acquireImageMemoryBarrier = {};
        acquireImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        acquireImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        acquireImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        acquireImageMemoryBarrier.image = swapchainImages[acquiredImageIdx];
        acquireImageMemoryBarrier.subresourceRange = acquireRange;

        //VkClearColorValue color = {static_cast<uint32_t>(red--), 0U, static_cast<uint32_t>(blue++), 0U};
        VkClearColorValue color = {red.operator++(), green.operator++(), blue.operator++(), 0.0};
        
        VkImageSubresourceRange range = {};
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        range.levelCount = 1;
        range.layerCount = 1;

        VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
        cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        res = vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo);
        if (res != VK_SUCCESS) {
            std::cerr << "Failed to begin command buffer\n";
            return -1;
        }

        vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                             VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr, 0, nullptr, 1, &acquireImageMemoryBarrier);

        vkCmdClearColorImage(cmdBuffer, swapchainImages[acquiredImageIdx],
                             VK_IMAGE_LAYOUT_GENERAL, &color, 1U, &range);

        VkImageSubresourceRange presentRange = {};
        presentRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        presentRange.levelCount = 1;
        presentRange.layerCount = 1;
        VkImageMemoryBarrier presentImageMemoryBarrier = {};
        presentImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        presentImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
        presentImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        presentImageMemoryBarrier.image = swapchainImages[acquiredImageIdx];
        presentImageMemoryBarrier.subresourceRange = presentRange;

        vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,
                             VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr, 0, nullptr, 1, &presentImageMemoryBarrier);

        res = vkEndCommandBuffer(cmdBuffer);
        if (res != VK_SUCCESS) {
            std::cerr << "Failed to end command buffer\n";
            return -1;
        }

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1U;
        submitInfo.pCommandBuffers = &cmdBuffer;
        res = vkQueueSubmit(queue, 1U, &submitInfo, queueSubmitPostFence);
        if (res != VK_SUCCESS) {
            std::cerr << "Failed to kickoff\n";
            return -1;
        }

        
        res = vkWaitForFences(dev, 1U, &queueSubmitPostFence, VK_TRUE, 1000000000U);
        if (res != VK_SUCCESS) {
            std::cerr << "Timed out waiting for post submit fence\n";
            return -1;
        }
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
