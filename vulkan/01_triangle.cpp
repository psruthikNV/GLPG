#include <vector>
#include <algorithm>

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#undef CreateWindow
#else
#define VK_USE_PLATFORM_XCB_KHR
#endif
#include <vulkan/vulkan.h>

#include "../include/GLPGWindow.hpp"
#include "../include/GLPGEvent.hpp"
#include "../include/utils/vk-print.hpp"
#include "../include/utils/vk-utils.hpp"

int main()
{
    GLPG::GLPGEventLoop eventLoop;
    GLPG::GLPGEvent event;
    constexpr uint32_t defaultDeviceIdx = 0U;
    constexpr uint32_t defaultWidth = 640U;
    constexpr uint32_t defaultHeight = 480U;
    uint32_t numPhysicalDevices = 0U;
    uint32_t numAvailableInstanceExtensions = 0U;
    uint32_t numRequiredInstanceExtensions = 0U;
    uint32_t numAvailableDeviceExtensions = 0U;
    uint32_t numRequiredDeviceExtensions = 0U;
    std::vector<const char*> requiredInstanceExtensions = {};
    std::vector<VkExtensionProperties> availableInstanceExtensions = {};
    std::vector<const char*> requiredLayers = {};
    requiredInstanceExtensions.push_back("VK_KHR_surface");
#ifdef _WIN32
    requiredInstanceExtensions.push_back("VK_KHR_win32_surface");
#else
    requiredInstanceExtensions.push_back("VK_KHR_xcb_surface");
#endif
#ifdef GLPG_IS_DEBUG
    requiredInstanceExtensions.push_back("VK_EXT_debug_utils");
    requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    std::vector<const char *> requiredDeviceExtensions = {};
    requiredDeviceExtensions.push_back("VK_KHR_swapchain");

    VkResult res = vkEnumerateInstanceExtensionProperties(nullptr, &numAvailableInstanceExtensions, nullptr);
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to get number of available instance extension properties\n";
        return -1;
    }
    availableInstanceExtensions.resize(numAvailableInstanceExtensions);

    res = vkEnumerateInstanceExtensionProperties(nullptr, &numAvailableInstanceExtensions, availableInstanceExtensions.data()); 
    if (res != VK_SUCCESS) {
        std::cerr << "Failed to get the available instance extensions\n";
        return -1;
    }

    bool hasRequiredExtensions;
    for (uint32_t idx = 0U; idx < requiredInstanceExtensions.size(); idx++) {
        hasRequiredExtensions = false;
        for (auto instanceExtension : availableInstanceExtensions) {
            if (hasRequiredExtensions) {
                break;
            }
            if (!strcmp(instanceExtension.extensionName, requiredInstanceExtensions[idx])) {
                hasRequiredExtensions = true;
                continue;
            }
        }
        if (!hasRequiredExtensions) {
            break;
        }
    }

    if (!hasRequiredExtensions) {
        std::cerr << "Required instance Extensions are not present\n";
        return -1;
    }
}
