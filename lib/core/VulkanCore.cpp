//
// Created by Guy on 05/06/2021.
//

#include <core/VulkanCore.hpp>

#include <iostream>
#include <fstream>
#include <set>
#include <map>
#include <cstdint> // Necessary for UINT32_MAX
#include <algorithm> // Necessary for std::min/std::max

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <data/Mesh.h>

#define TEST(x) x != vk::Result::eSuccess
#define TEST_EXC(x, msg) if(x != vk::Result::eSuccess) throw std::runtime_error(msg);

#ifdef NDEBUG
#define IN_DEBUG(x)
#define CHECK(x) x
const bool enableValidationLayers = true;
#else
#define IN_DEBUG(x) x,
#define CHECK(x) if(x != vk::Result::eSuccess) throw std::runtime_error(#x);
const bool enableValidationLayers = true;
#endif

const int MAX_FRAMES_IN_FLIGHT = 3;

extern std::string CWD;

const std::vector<const char*> validationLayers = {
        IN_DEBUG("VK_LAYER_KHRONOS_validation")
        "VK_LAYER_LUNARG_monitor"
};

const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto core = reinterpret_cast<VulkanCore*>(glfwGetWindowUserPointer(window));
    core->framebufferResized = true;
}

static VkResult CreateDebugUtilsMessengerEXT(vk::Instance instance, const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo, const vk::AllocationCallbacks* pAllocator, vk::DebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr((VkInstance) instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
        return func((VkInstance) instance, (VkDebugUtilsMessengerCreateInfoEXT*) pCreateInfo, (VkAllocationCallbacks*) pAllocator, (VkDebugUtilsMessengerEXT*) pDebugMessenger);
    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

static void DestroyDebugUtilsMessengerEXT(vk::Instance instance, vk::DebugUtilsMessengerEXT debugMessenger, const vk::AllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr((VkInstance) instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
        func((VkInstance) instance, (VkDebugUtilsMessengerEXT) debugMessenger, (VkAllocationCallbacks*) pAllocator);
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    }

    return VK_FALSE;
}

static std::vector<char> ReadFile(const std::string& filename) {
    std::string p = (CWD + filename) + ".spv";
    std::cout << "Opening file: " << p.c_str() << " for reading" << std::endl;
    std::ifstream file(p, std::ios::ate | std::ios::binary);
    if(!file.is_open())
        throw std::runtime_error("Failed to open file");

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

std::vector<const char*> VulkanCore::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

bool VulkanCore::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
            return false;
    }

    return true;
}

void VulkanCore::PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo.setMessageSeverity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo);
    createInfo.setMessageType(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
    createInfo.setPfnUserCallback(debugCallback);
    createInfo.setPUserData(nullptr);
}

void VulkanCore::CreateInstance() {
    if(enableValidationLayers && !checkValidationLayerSupport())
        throw std::runtime_error("Validation layers requested, but not available");

    vk::ApplicationInfo appInfo{};
    appInfo.setPApplicationName("VK Model Viewer");
    appInfo.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0));
    appInfo.setPEngineName("Model Engine");
    appInfo.setEngineVersion(VK_MAKE_VERSION(1, 0, 0));
    appInfo.setApiVersion(VK_API_VERSION_1_2);

    vk::InstanceCreateInfo instanceCreateInfo{};
    instanceCreateInfo.setPApplicationInfo(&appInfo);

    auto extensions = getRequiredExtensions();
    instanceCreateInfo.setEnabledExtensionCount(extensions.size());
    instanceCreateInfo.setPpEnabledExtensionNames(extensions.data());

    instanceCreateInfo.setEnabledLayerCount(0);

    vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if(enableValidationLayers) {
        instanceCreateInfo.setPEnabledLayerNames(validationLayers);
//        instanceCreateInfo.setEnabledLayerCount(validationLayers.size());
//        instanceCreateInfo.setPpEnabledLayerNames(validationLayers.data());
        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        instanceCreateInfo.setPNext(&debugCreateInfo);
    }else{
        instanceCreateInfo.setEnabledLayerCount(0);
    }

    vk::Result result = vk::createInstance(&instanceCreateInfo, nullptr, &instance);
    if(result != vk::Result::eSuccess)
        throw std::runtime_error("Failed to create vk::Instance");
}

void VulkanCore::SetupDebugMessenger() {
    if(!enableValidationLayers) return;

    vk::DebugUtilsMessengerCreateInfoEXT createInfo{};
    PopulateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

GLFWwindow* VulkanCore::InitVulkan() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(800, 600, "VK Model Viewer", nullptr, nullptr);
    if(window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    uint32_t extensionCount = 0;
    vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::cout << "Extensions supported: " << extensionCount << std::endl;
    std::vector<vk::ExtensionProperties> extensions(extensionCount);
    vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    std::cout << "Available extensions:\n";

    for (const auto& extension : extensions)
        std::cout << '\t' << extension.extensionName << std::endl;

    CreateInstance();
    SetupDebugMessenger();
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();
    CreateSwapChain();
    CreateImageViews();
    CreateRenderPass();
    CreateGraphicsPipeline();
    CreateFramebuffers();
    CreateCommandPool();
    CreateCommandBuffers();
    CreateSyncObjects();

    return window;
}

void VulkanCore::Cleanup() {
    if(enableValidationLayers)
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        device.destroySemaphore(renderFinishedSemaphores[i]);
        device.destroySemaphore(imageAvailableSemaphores[i]);
        device.destroyFence(inFlightFences[i]);
    }

    CleanupSwapchain();

    for(const auto& mesh : meshes)
        mesh->Dispose();
    device.destroyCommandPool(commandPool);

    device.destroy();
    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
    instance.destroySurfaceKHR(surface);
    instance.destroy();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void VulkanCore::PickPhysicalDevice() {
    std::vector<vk::PhysicalDevice, std::allocator<vk::PhysicalDevice>> devices = instance.enumeratePhysicalDevices();
    if(devices.size() == 0)
        throw std::runtime_error("Failed to find GPUs with Vulkan support");

    std::multimap<int, vk::PhysicalDevice> candidates;
    for(const auto& device : devices) {
        int score = RateDeviceSuitability(device);
        candidates.insert(std::make_pair(score, device));
    }

    if(candidates.rbegin()->first > 0)
        physicalDevice = candidates.rbegin()->second;
    else
        throw std::runtime_error("Failed to find a suitable GPU");
}

bool VulkanCore::CheckDeviceExtensionSupport(vk::PhysicalDevice device) {
    std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties();
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for(const auto& extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
}

int VulkanCore::RateDeviceSuitability(vk::PhysicalDevice device) {
    vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
    vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

    if(!CheckDeviceExtensionSupport(device))
        return 0;

    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
    if(swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty())
        return 0;

    if(!deviceFeatures.geometryShader)
        return 0;

    if(!FindQueueFamilies(device).IsComplete())
        return 0;

    int score = 0;

    if(deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
        score += 1024;
    score += deviceProperties.limits.maxImageDimension2D;
    return score;
}

QueueFamilyIndices VulkanCore::FindQueueFamilies(vk::PhysicalDevice device) {
    QueueFamilyIndices indices{};

    std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();


    int i = 0;
    for(const auto& queueFamily : queueFamilies) {
        if(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
            indices.graphicsFamily = i;

        vk::Bool32 presentSupport = false;
        device.getSurfaceSupportKHR(i, surface, &presentSupport);
        if(presentSupport)
            indices.presentFamily = i;

        if(indices.IsComplete())
            break;
        i++;
    }

    return indices;
}

void VulkanCore::CreateLogicalDevice() {
    QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for(uint32_t queueFamily : uniqueQueueFamilies) {
        vk::DeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.setQueueFamilyIndex(queueFamily);
        queueCreateInfo.setQueueCount(1);
        queueCreateInfo.setQueuePriorities(queuePriority);
        queueCreateInfos.push_back(queueCreateInfo);
    }

    vk::PhysicalDeviceFeatures deviceFeatures = physicalDevice.getFeatures();

    vk::DeviceCreateInfo createInfo{};
    createInfo.setQueueCreateInfoCount(queueCreateInfos.size());
    createInfo.setPQueueCreateInfos(queueCreateInfos.data());
    createInfo.setPEnabledFeatures(&deviceFeatures);
    createInfo.setEnabledExtensionCount(deviceExtensions.size());
    createInfo.setPpEnabledExtensionNames(deviceExtensions.data());

    if (enableValidationLayers) {
        createInfo.setEnabledLayerCount(validationLayers.size());
        createInfo.setPpEnabledLayerNames(validationLayers.data());
    } else {
        createInfo.setEnabledLayerCount(0);
    }

    if(physicalDevice.createDevice(&createInfo, nullptr, &device) != vk::Result::eSuccess)
        throw std::runtime_error("Failed to create logical device");

    device.getQueue(indices.graphicsFamily.value(), 0, &graphicsQueue);
    device.getQueue(indices.presentFamily.value(), 0, &presentQueue);
}

void VulkanCore::CreateSurface() {
    if(glfwCreateWindowSurface((VkInstance) instance, window, nullptr, ((VkSurfaceKHR*) &surface)) != VK_SUCCESS)
        throw std::runtime_error("Failed to create window surface");
}

SwapChainSupportDetails VulkanCore::QuerySwapChainSupport(vk::PhysicalDevice device) {
    SwapChainSupportDetails details{};
    details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
    details.formats = device.getSurfaceFormatsKHR(surface);
    details.presentModes = device.getSurfacePresentModesKHR(surface);
    return details;
}

vk::SurfaceFormatKHR VulkanCore::ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
    for(const auto& availableFormat : availableFormats) {
        if(availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
            return availableFormat;
    }
    return availableFormats[0];
}

vk::PresentModeKHR VulkanCore::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if(availablePresentMode == vk::PresentModeKHR::eMailbox)
            return availablePresentMode;
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D VulkanCore::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities) {
    if(capabilities.currentExtent.width != UINT32_MAX)
        return capabilities.currentExtent;
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    
    vk::Extent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
    };

    actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

    return actualExtent;
}

void VulkanCore::CreateSwapChain() {
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice);

    vk::SurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
    vk::Extent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if(swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
        imageCount = swapChainSupport.capabilities.maxImageCount;

    vk::SwapchainCreateInfoKHR createInfo{};
    createInfo.setSurface(surface);
    createInfo.setMinImageCount(imageCount);
    createInfo.setImageFormat(surfaceFormat.format);
    createInfo.setImageColorSpace(surfaceFormat.colorSpace);
    createInfo.setImageExtent(extent);
    createInfo.setImageArrayLayers(1);
    createInfo.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

    QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    if (device.createSwapchainKHR(&createInfo, nullptr, &swapchain) != vk::Result::eSuccess)
        throw std::runtime_error("Failed to create swap chain");

    swapchainImages = device.getSwapchainImagesKHR(swapchain);
    swapchainImageFormat = surfaceFormat.format;
    swapchainExtent = extent;
}

void VulkanCore::CreateImageViews() {
    swapchainImageViews.resize(swapchainImages.size());
    for(size_t i = 0; i < swapchainImages.size(); i++) {
        vk::ImageViewCreateInfo createInfo{};
        createInfo.setImage(swapchainImages[i]);
        createInfo.setViewType(vk::ImageViewType::e2D);
        createInfo.setFormat(swapchainImageFormat);
        createInfo.components.setR(vk::ComponentSwizzle::eIdentity);
        createInfo.components.setG(vk::ComponentSwizzle::eIdentity);
        createInfo.components.setB(vk::ComponentSwizzle::eIdentity);
        createInfo.components.setA(vk::ComponentSwizzle::eIdentity);
        createInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
        createInfo.subresourceRange.setBaseMipLevel(0);
        createInfo.subresourceRange.setLevelCount(1);
        createInfo.subresourceRange.setBaseArrayLayer(0);
        createInfo.subresourceRange.setLayerCount(1);

        if(device.createImageView(&createInfo, nullptr, &swapchainImageViews[i]) != vk::Result::eSuccess)
            throw std::runtime_error("Failed to create image views");
    }
}

void VulkanCore::CreateGraphicsPipeline() {
    std::vector<char> vertShaderCode = ReadFile("assets/shaders/sample/sample.vert");
    std::vector<char> fragShaderCode = ReadFile("assets/shaders/sample/sample.frag");

    vk::ShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
    vk::ShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.setStage(vk::ShaderStageFlagBits::eVertex);
    vertShaderStageInfo.setModule(vertShaderModule);
    vertShaderStageInfo.setPName("main");

    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.setStage(vk::ShaderStageFlagBits::eFragment);
    fragShaderStageInfo.setModule(fragShaderModule);
    fragShaderStageInfo.setPName("main");

    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescription = Vertex::getAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.setVertexBindingDescriptionCount(1);
    vertexInputInfo.setPVertexBindingDescriptions(&bindingDescription);
    vertexInputInfo.setVertexAttributeDescriptions(attributeDescription);

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
    inputAssembly.setPrimitiveRestartEnable(VK_FALSE);

    vk::Viewport viewport{};
    viewport.setX(0).setY(0);
    viewport.setWidth((float) swapchainExtent.width).setHeight((float) swapchainExtent.height);
    viewport.setMinDepth(0).setMaxDepth(1);

    vk::Rect2D scissor{};
    scissor.setOffset({0, 0});
    scissor.setExtent(swapchainExtent);

    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.setViewportCount(1);
    viewportState.setPViewports(&viewport);
    viewportState.setScissorCount(1);
    viewportState.setPScissors(&scissor);

    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.setDepthClampEnable(VK_FALSE);
    rasterizer.setRasterizerDiscardEnable(VK_FALSE);
    rasterizer.setPolygonMode(vk::PolygonMode::eFill);
    rasterizer.setLineWidth(1);
    rasterizer.setCullMode(vk::CullModeFlagBits::eBack);
    rasterizer.setFrontFace(vk::FrontFace::eClockwise);
    rasterizer.setDepthBiasEnable(VK_FALSE);

    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.setSampleShadingEnable(VK_FALSE);
    multisampling.setRasterizationSamples(vk::SampleCountFlagBits::e1);

    // Per-framebuffer blending settings
    vk::PipelineColorBlendAttachmentState colourBlendAttachment{};
    colourBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    colourBlendAttachment.setBlendEnable(VK_TRUE);
    colourBlendAttachment.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
    colourBlendAttachment.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
    colourBlendAttachment.setColorBlendOp(vk::BlendOp::eAdd);
    colourBlendAttachment.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
    colourBlendAttachment.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
    colourBlendAttachment.setAlphaBlendOp(vk::BlendOp::eAdd);

    // Global blending settings
    vk::PipelineColorBlendStateCreateInfo colourBlending{};
    colourBlending.setLogicOpEnable(VK_FALSE);
    colourBlending.setAttachmentCount(1);
    colourBlending.setPAttachments(&colourBlendAttachment);

    vk::DynamicState dynamicStates[] = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eLineWidth
    };

    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.setDynamicStateCount(2);
    dynamicState.setPDynamicStates(dynamicStates);

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};

    if(device.createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipelineLayout) != vk::Result::eSuccess)
        throw std::runtime_error("Failed to create pipeline layout");

    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.setStageCount(2);
    pipelineInfo.setPStages(shaderStages);
    pipelineInfo.setPVertexInputState(&vertexInputInfo);
    pipelineInfo.setPInputAssemblyState(&inputAssembly);
    pipelineInfo.setPViewportState(&viewportState);
    pipelineInfo.setPRasterizationState(&rasterizer);
    pipelineInfo.setPMultisampleState(&multisampling);
    pipelineInfo.setPDepthStencilState(nullptr);
    pipelineInfo.setPColorBlendState(&colourBlending);
//    pipelineInfo.setPDynamicState(&dynamicState);
    pipelineInfo.setLayout(pipelineLayout);
    pipelineInfo.setRenderPass(renderPass);
    pipelineInfo.setSubpass(0);

    if(device.createGraphicsPipelines(nullptr, 1, &pipelineInfo, nullptr, &graphicsPipeline) != vk::Result::eSuccess)
        throw std::runtime_error("Failed to create graphics pipeline");

    device.destroyShaderModule(vertShaderModule);
    device.destroyShaderModule(fragShaderModule);
}

vk::ShaderModule VulkanCore::CreateShaderModule(const std::string& file) {
    return CreateShaderModule(ReadFile(file));
}

vk::ShaderModule VulkanCore::CreateShaderModule(std::vector<char> source) {
    vk::ShaderModuleCreateInfo createInfo{};
    createInfo.setCodeSize(source.size());
    createInfo.setPCode(reinterpret_cast<const uint32_t *>(source.data()));

    vk::ShaderModule shaderModule;
    if(device.createShaderModule(&createInfo, nullptr, &shaderModule) != vk::Result::eSuccess)
        throw std::runtime_error("Failed to create shader module");
    return shaderModule;
}

void VulkanCore::CreateRenderPass(){
    vk::AttachmentDescription colourAttachment{};
    colourAttachment.setFormat(swapchainImageFormat);
    colourAttachment.setSamples(vk::SampleCountFlagBits::e1);
    colourAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
    colourAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);
    colourAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    colourAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    colourAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
    colourAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    vk::AttachmentReference colourAttachmentRef{};
    colourAttachmentRef.setAttachment(0);
    colourAttachmentRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription subpass{};
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
    subpass.setColorAttachmentCount(1);
    subpass.setPColorAttachments(&colourAttachmentRef);

    vk::SubpassDependency dependencies[2];
    dependencies[0].setSrcSubpass(VK_SUBPASS_EXTERNAL);
    dependencies[0].setDstSubpass(0);
    dependencies[0].setSrcStageMask(vk::PipelineStageFlagBits::eBottomOfPipe);
    dependencies[0].setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    dependencies[0].setSrcAccessMask(vk::AccessFlagBits::eMemoryRead);
    dependencies[0].setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);

    dependencies[1].setSrcSubpass(0);
    dependencies[1].setDstSubpass(VK_SUBPASS_EXTERNAL);
    dependencies[1].setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    dependencies[1].setDstStageMask(vk::PipelineStageFlagBits::eBottomOfPipe);
    dependencies[1].setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);
    dependencies[1].setDstAccessMask(vk::AccessFlagBits::eMemoryRead);

    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.setAttachmentCount(1);
    renderPassInfo.setPAttachments(&colourAttachment);
    renderPassInfo.setSubpassCount(1);
    renderPassInfo.setPSubpasses(&subpass);
    renderPassInfo.setDependencyCount(2);
    renderPassInfo.setPDependencies(dependencies);

    if(device.createRenderPass(&renderPassInfo, nullptr, &renderPass) != vk::Result::eSuccess)
        throw std::runtime_error("Failed to create render pass");

}

void VulkanCore::CreateFramebuffers(){
    swapchainFramebuffers.resize(swapchainImageViews.size());
    for(size_t i = 0; i < swapchainImageViews.size(); i++) {
        vk::ImageView attachments[] = {
                swapchainImageViews[i]
        };

        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.setRenderPass(renderPass);
        framebufferInfo.setAttachmentCount(1);
        framebufferInfo.setPAttachments(attachments);
        framebufferInfo.setWidth(swapchainExtent.width).setHeight(swapchainExtent.height);
        framebufferInfo.setLayers(1);

        if(device.createFramebuffer(&framebufferInfo, nullptr, &swapchainFramebuffers[i]) != vk::Result::eSuccess)
            throw std::runtime_error("Failed to create framebuffer");
    }
}

void VulkanCore::CreateCommandPool(){
    QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physicalDevice);

    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.setQueueFamilyIndex(queueFamilyIndices.graphicsFamily.value());

    if(device.createCommandPool(&poolInfo, nullptr, &commandPool) != vk::Result::eSuccess)
        throw std::runtime_error("Failed to create command pool");
}

void VulkanCore::CreateCommandBuffers() {
    commandBuffers.resize(swapchainFramebuffers.size());

    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.setCommandPool(commandPool);
    allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    allocInfo.setCommandBufferCount(commandBuffers.size());

    if(device.allocateCommandBuffers(&allocInfo, commandBuffers.data()) != vk::Result::eSuccess)
        throw std::runtime_error("Failed to allocate command buffers");

    for(size_t i = 0; i < commandBuffers.size(); i++) {
        vk::CommandBufferBeginInfo beginInfo{};

        if(TEST(commandBuffers[i].begin(&beginInfo)))
            throw std::runtime_error("Failed to begin recording command buffer");

        vk::RenderPassBeginInfo renderPassInfo{};
        renderPassInfo.setRenderPass(renderPass);
        renderPassInfo.setFramebuffer(swapchainFramebuffers[i]);
        renderPassInfo.renderArea.setOffset({0, 0});
        renderPassInfo.renderArea.setExtent(swapchainExtent);
        vk::ClearValue clearColour{};
        clearColour.color.setFloat32({0, 0, 0, 1});
        renderPassInfo.setClearValueCount(1);
        renderPassInfo.setPClearValues(&clearColour);

        commandBuffers[i].beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);
        commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);

        for(const auto& mesh : meshes) {
            mesh->CreateVertexBuffer(this, &device);
            vk::Buffer vertexBuffers[] = {mesh->GetVertexBuffer()};
            vk::DeviceSize offsets[] = {0};
            commandBuffers[i].bindVertexBuffers(0, 1, vertexBuffers, offsets);
            uint32_t vertexCount = mesh->GetVertexCount();
            commandBuffers[i].draw(vertexCount, 1, 0, 0);
        }

        commandBuffers[i].endRenderPass();

        commandBuffers[i].end();
    }
}

void VulkanCore::CreateSyncObjects() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(swapchainImages.size(), VK_NULL_HANDLE);
    vk::SemaphoreCreateInfo semaphoreInfo{};
    vk::FenceCreateInfo fenceInfo{};
    fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        CHECK(device.createSemaphore(&semaphoreInfo, nullptr, &imageAvailableSemaphores[i]));
        CHECK(device.createSemaphore(&semaphoreInfo, nullptr, &renderFinishedSemaphores[i]));
        CHECK(device.createFence(&fenceInfo, nullptr, &inFlightFences[i]));
    }

//    if (TEST(device.createSemaphore(&semaphoreInfo, nullptr, &imageAvailableSemaphores)) ||
//        TEST(device.createSemaphore(&semaphoreInfo, nullptr, &renderFinishedSemaphores)))
//        throw std::runtime_error("Failed to create semaphores");
}

void VulkanCore::MainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        DrawFrame();
    }

    device.waitIdle();
}

void VulkanCore::DrawFrame() {

    uint32_t imageIndex;
    vk::Result result = device.acquireNextImageKHR(swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == vk::Result::eErrorOutOfDateKHR) {
        RecreateSwapchain();
        return;
    } else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    if(imagesInFlight[imageIndex] != VK_NULL_HANDLE)
        CHECK(device.waitForFences(1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX));
    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    vk::SubmitInfo submitInfo{};
    vk::Semaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitInfo.setWaitSemaphoreCount(1);
    submitInfo.setPWaitSemaphores(waitSemaphores);
    submitInfo.setPWaitDstStageMask(waitStages);
    vk::Semaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.setSignalSemaphoreCount(1);
    submitInfo.setPSignalSemaphores(signalSemaphores);
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&commandBuffers[imageIndex]);

    CHECK(device.resetFences(1, &inFlightFences[currentFrame]));

    CHECK(graphicsQueue.submit(1, &submitInfo, inFlightFences[currentFrame]));

    vk::PresentInfoKHR presentInfo{};
    presentInfo.setWaitSemaphoreCount(1);
    presentInfo.setPWaitSemaphores(signalSemaphores);

    vk::SwapchainKHR swapchains[] = {swapchain};
    presentInfo.setSwapchainCount(1);
    presentInfo.setPSwapchains(swapchains);
    presentInfo.setPImageIndices(&imageIndex);

    result = presentQueue.presentKHR(&presentInfo);

    if(result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || framebufferResized) {
        framebufferResized = false;
        RecreateSwapchain();
        return;
    }
    CHECK(result);

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanCore::CleanupSwapchain() {
    for(auto framebuffer : swapchainFramebuffers)
        device.destroyFramebuffer(framebuffer);

    device.freeCommandBuffers(commandPool, commandBuffers.size(), commandBuffers.data());

    device.destroyPipeline(graphicsPipeline);
    device.destroyPipelineLayout(pipelineLayout);
    device.destroyRenderPass(renderPass);
    for (auto imageView : swapchainImageViews)
        device.destroyImageView(imageView);
    device.destroySwapchainKHR(swapchain);
}

void VulkanCore::RecreateSwapchain() {
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    device.waitIdle();

    CleanupSwapchain();

    CreateSwapChain();
    CreateImageViews();
    CreateRenderPass();
    CreateGraphicsPipeline();
    CreateFramebuffers();
    CreateCommandBuffers();
}

void VulkanCore::AllocateMemory(vk::Buffer& buffer, vk::DeviceMemory *memory) {
    vk::MemoryRequirements memRequirements;
    device.getBufferMemoryRequirements(buffer, &memRequirements);

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    CHECK(device.allocateMemory(&allocInfo, nullptr, memory));
    device.bindBufferMemory(buffer, *memory, 0);
}

uint32_t VulkanCore::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

    for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if(typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }
    throw std::runtime_error("Failed to find suitable memory type");
}
void VulkanCore::AddMesh(std::shared_ptr<Mesh> mesh) {
    meshes.push_back(mesh);
}
