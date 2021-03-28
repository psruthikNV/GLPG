#include <iostream>
#include <vulkan/vulkan.hpp>

// Manually written. Switch to the generated vk-print headers
// once they're ready for use.

static void VkPrintVkExtent2D(const VkExtent2D &type)
{
    std::cout << "VkExtent2D.width: " << type.width << std::endl;
    std::cout << "VkExtent2D.height: " << type.height << std::endl;
}

static void ParseDeviceQueueDetails(uint32_t numDeviceQueues, VkQueueFamilyProperties *queuePropArray)
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
static void VkPrintVkSurfaceTransformFlagsKHR(const VkSurfaceTransformFlagsKHR &type)
{
    if (type & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        std::cout << "VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR\n";
    }

    if (type & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR) {
        std::cout << "VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR\n";
    }

    if (type & VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR) {
        std::cout << "VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR\n";
    }
    
    if (type & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
        std::cout << "VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR\n";
    }

    if (type & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR) {
        std::cout << "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR\n";
    }

    if (type & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR) {
        std::cout << "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR\n";
    }

    if (type & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR) {
        std::cout << "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR\n";
    }

    if (type & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR) {
        std::cout << "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR\n";
    }

    if (type & VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR) {
        std::cout << "VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR\n";
    }

}

static void VkPrintVkSurfaceTransformFlagBitsKHR(const VkSurfaceTransformFlagBitsKHR &type)
{
    if (type == VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        std::cout << "VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR\n";
    }

    if (type == VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR) {
        std::cout << "VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR\n";
    }

    if (type == VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR) {
        std::cout << "VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR\n";
    }
    
    if (type == VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) {
        std::cout << "VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR\n";
    }

    if (type == VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR) {
        std::cout << "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR\n";
    }

    if (type == VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR) {
        std::cout << "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR\n";
    }

    if (type == VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR) {
        std::cout << "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR\n";
    }

    if (type == VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR) {
        std::cout << "VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR\n";
    }

    if (type == VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR) {
        std::cout << "VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR\n";
    }
    
}

static void VkPrintVkCompositeAlphaFlagsKHR(const VkCompositeAlphaFlagsKHR &type)
{
    if (type & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) {
        std::cout << "VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR\n";
    }

    if (type & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {
        std::cout << "VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR\n";
    }

    if (type & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) {
        std::cout << "VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR\n";
    }

    if (type & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR ) {
        std::cout << "VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR\n";
    }

}

static void VkPrintVkImageUsageFlags(const VkImageUsageFlags &type)
{
    if (type & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
        std::cout << "VK_IMAGE_USAGE_TRANSFER_SRC_BIT\n";
    }

    if (type & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
        std::cout << "VK_IMAGE_USAGE_TRANSFER_DST_BIT\n";
    }

    if (type & VK_IMAGE_USAGE_SAMPLED_BIT) {
        std::cout << "VK_IMAGE_USAGE_SAMPLED_BIT\n";
    }

    if (type & VK_IMAGE_USAGE_STORAGE_BIT) {
        std::cout << "VK_IMAGE_USAGE_STORAGE_BIT\n";
    }

    if (type & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
        std::cout << "VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT\n";
    }

    if (type & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        std::cout << "VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT\n";
    }

    if (type & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT) {
        std::cout << "VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT\n";
    }

    if (type & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT) {
        std::cout << "VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT\n";
    }

    if (type & VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV) {
        std::cout << "VK_IMAGE_USAGE_SHADING_RATE_IMAGE_BIT_NV\n";
    }

    if (type & VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT) {
        std::cout << "VK_IMAGE_USAGE_FRAGMENT_DENSITY_MAP_BIT_EXT\n";
    }

    if (type & VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR) {
        std::cout << "VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR\n";
    }
}

static void VkPrintVkSurfaceCapabilitiesKHR(const VkSurfaceCapabilitiesKHR &s)
{
    std::cout << "VkSurfaceCapabilitiesKHR.minImageCount: " << s.minImageCount << std::endl;
    std::cout << "VkSurfaceCapabilitiesKHR.maxImageCount: " << s.maxImageCount << std::endl;
    std::cout << "VkSurfaceCapabilitiesKHR.currentExtent: \n";
    VkPrintVkExtent2D(s.currentExtent);
    std::cout << "VkSurfaceCapabilitiesKHR.minImageExtent: \n";
    VkPrintVkExtent2D(s.minImageExtent);
    std::cout << "VkSurfaceCapabilitiesKHR.maxImageExtent: \n";
    VkPrintVkExtent2D(s.maxImageExtent);
    std::cout << "VkSurfaceCapabilitiesKHR.maxImageArrayLayers: " << s.maxImageArrayLayers << std::endl;
    std::cout << "VkSurfaceCapabilitiesKHR.supportedTransforms: \n";
    VkPrintVkSurfaceTransformFlagsKHR(s.supportedTransforms);
    std::cout << "VkSurfaceCapabilitiesKHR.currentTransform: \n";
    VkPrintVkSurfaceTransformFlagBitsKHR(s.currentTransform);
    std::cout << "VkSurfaceCapabilitiesKHR.supportedCompositeAlpha: \n";
    VkPrintVkCompositeAlphaFlagsKHR(s.supportedCompositeAlpha);
    std::cout << "VkSurfaceCapabilitiesKHR.supportedUsageFlags: \n";
    VkPrintVkImageUsageFlags(s.supportedUsageFlags);
}

static void VkPrintVkLayerProperties(const VkLayerProperties &type)
{
    std::cout << "VkLayerProperties.layerName: " << type.layerName << "\n";
    std::cout << "VkLayerProperties.specVersion: " << type.specVersion << "\n";
    std::cout << "VkLayerProperties.implementationVersion: " << type.implementationVersion << "\n";
    std::cout << "VkLayerProperties.description: " << type.description << "\n";
}