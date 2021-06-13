//
// Created by Guy on 05/06/2021.
//
#include <pch.h>

#include <core/VulkanCore.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

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
#include <data/vk/CommandBuffer.h>

#define TEST(x) x != vk::Result::eSuccess
#define TEST_EXC(x, msg) if(x != vk::Result::eSuccess) throw std::runtime_error(msg);

#ifdef NDEBUG
#define IN_DEBUG(x)
const bool enableValidationLayers = true;
#else
#define IN_DEBUG(x) x,
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

extern std::vector<char> ReadFile(const std::string& filename);

template<typename T, typename U>
static std::map<T, std::vector<U>> GroupMeshes(std::vector<U> values, std::function<T(U)> keyGetter) {
    std::map<T, std::vector<U>> map;
    for (const U& item : values) {
        T key = keyGetter(item);
        if(!map.contains(key))
            map.emplace(key, std::vector<U>());
        map.at(key).push_back(item);
    }
    return map;
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
    CHECK(vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr));
    std::cout << "Extensions supported: " << extensionCount << std::endl;
    std::vector<vk::ExtensionProperties> extensions(extensionCount);
    CHECK(vk::enumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data()));

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
    CreateDescriptorPool();
    CreateUniformBuffers();
    CreateGraphicsPipeline();
    CreateCommandPool();
    CreateDepthResources();
    CreateFramebuffers();
    CreateCommandBuffers();
    CreateSyncObjects();

    staticMeshAdded += (staticMeshAddedHandle = [](VulkanCore* core, std::shared_ptr<Mesh> meshPtr) {
      core->RecreateStaticCommandBuffer();
    });

    return window;
}

void VulkanCore::Cleanup() {

    staticMeshAdded -= staticMeshAddedHandle;

    if(enableValidationLayers)
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        device.destroySemaphore(renderFinishedSemaphores[i]);
        device.destroySemaphore(imageAvailableSemaphores[i]);
        device.destroyFence(inFlightFences[i]);
    }

    depthTexturePtr.reset();

    CleanupSwapchain();

    for(const auto& mesh : staticMeshes)
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
        CHECK(device.getSurfaceSupportKHR(i, surface, &presentSupport));
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

        swapchainImageViews[i] =  CreateImageView(swapchainImages[i], swapchainImageFormat, vk::ImageAspectFlagBits::eColor);

//        vk::ImageViewCreateInfo createInfo{};
//        createInfo.setImage(swapchainImages[i]);
//        createInfo.setViewType(vk::ImageViewType::e2D);
//        createInfo.setFormat(swapchainImageFormat);
//        createInfo.components.setR(vk::ComponentSwizzle::eIdentity);
//        createInfo.components.setG(vk::ComponentSwizzle::eIdentity);
//        createInfo.components.setB(vk::ComponentSwizzle::eIdentity);
//        createInfo.components.setA(vk::ComponentSwizzle::eIdentity);
//        createInfo.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
//        createInfo.subresourceRange.setBaseMipLevel(0);
//        createInfo.subresourceRange.setLevelCount(1);
//        createInfo.subresourceRange.setBaseArrayLayer(0);
//        createInfo.subresourceRange.setLayerCount(1);
//
//        if(device.createImageView(&createInfo, nullptr, &swapchainImageViews[i]) != vk::Result::eSuccess)
//            throw std::runtime_error("Failed to create image views");
    }
}

void VulkanCore::CreateGraphicsPipeline() {
    std::vector<char> vertShaderCode = ReadFile("assets/shaders/sample/sample.vert");
    std::vector<char> fragShaderCode = ReadFile("assets/shaders/sample/sample.frag");

//    defaultShaderProgram = CreateShaderProgram();
//    defaultShaderProgram->AddStage(ShaderStage("main", vk::ShaderStageFlagBits::eVertex, vertShaderCode));
//    defaultShaderProgram->AddStage(ShaderStage("main", vk::ShaderStageFlagBits::eFragment, fragShaderCode));
//    defaultShaderProgram->Compile(swapchainExtent, swapchainImageFormat, descriptorPool);
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

    vk::AttachmentDescription depthAttachment{};
    depthAttachment.setFormat(FindDepthFormat());
    depthAttachment.setSamples(vk::SampleCountFlagBits::e1);
    depthAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
    depthAttachment.setStoreOp(vk::AttachmentStoreOp::eDontCare);
    depthAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    depthAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    depthAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
    depthAttachment.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::AttachmentReference depthAttachmentRef{};
    depthAttachmentRef.setAttachment(1);
    depthAttachmentRef.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::SubpassDescription subpass{};
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
    subpass.setColorAttachmentCount(1);
    subpass.setPColorAttachments(&colourAttachmentRef);
    subpass.setPDepthStencilAttachment(&depthAttachmentRef);

//    std::array<vk::SubpassDependency, 2> dependencies;
    std::array<vk::SubpassDependency, 1> dependencies;
    dependencies[0].setSrcSubpass(VK_SUBPASS_EXTERNAL);
    dependencies[0].setDstSubpass(0);
    dependencies[0].setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests);
    dependencies[0].setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests);
    dependencies[0].setSrcAccessMask(vk::AccessFlagBits::eNoneKHR);
    dependencies[0].setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

//    dependencies[1].setSrcSubpass(0);
//    dependencies[1].setDstSubpass(VK_SUBPASS_EXTERNAL);
//    dependencies[1].setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
//    dependencies[1].setDstStageMask(vk::PipelineStageFlagBits::eBottomOfPipe);
//    dependencies[1].setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);
//    dependencies[1].setDstAccessMask(vk::AccessFlagBits::eMemoryRead);

    std::array<vk::AttachmentDescription, 2> attachments = { colourAttachment, depthAttachment };
    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.setAttachments(attachments);
    renderPassInfo.setSubpassCount(1);
    renderPassInfo.setPSubpasses(&subpass);
    renderPassInfo.setDependencies(dependencies);

    if(device.createRenderPass(&renderPassInfo, nullptr, &renderPass) != vk::Result::eSuccess)
        throw std::runtime_error("Failed to create render pass");

}

void VulkanCore::CreateFramebuffers(){
    swapchainFramebuffers.resize(swapchainImageViews.size());
    for(size_t i = 0; i < swapchainImageViews.size(); i++) {
        std::array<vk::ImageView, 2> attachments = {
                swapchainImageViews[i],
                depthTexturePtr->GetView()
        };

        vk::FramebufferCreateInfo framebufferInfo{};
        framebufferInfo.setRenderPass(renderPass);
        framebufferInfo.setAttachments(attachments);
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
        std::array<vk::ClearValue, 2> clearValues{};
        clearValues[0].color.setFloat32({0, 0, 0, 1});
        clearValues[1].depthStencil.setDepth(1.0f).setStencil(0);
        renderPassInfo.setClearValues(clearValues);

        commandBuffers[i].beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

        std::map<std::shared_ptr<ShaderProgram>, std::vector<std::shared_ptr<Mesh>>> meshMap;
        for (const std::shared_ptr<Mesh>& item : staticMeshes) {
            std::shared_ptr<ShaderProgram> key = item->GetShaderProgram();
            meshMap[key].push_back(item);
        }

        for(const auto& pair : meshMap) {
            std::shared_ptr<ShaderProgram> shader;
//            if(pair.first == nullptr)
//                shader = defaultShaderProgram;
//            else
                shader = pair.first;

            commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, *shader->GetPipeline());
            shader->BindDescriptorSet(commandBuffers[i], vk::PipelineBindPoint::eGraphics, i);

            for(const auto& mesh : pair.second) {
                mesh->CreateVertexBuffer(*this, device);
                mesh->CreateIndexBuffer(*this, device);
                vk::Buffer vertexBuffers[] = {mesh->GetVertexBuffer()};
                vk::DeviceSize offsets[] = {0};
                commandBuffers[i].bindVertexBuffers(0, 1, vertexBuffers, offsets);
                commandBuffers[i].bindIndexBuffer(mesh->GetIndexBuffer(), 0, mesh->GetIndexType());
                commandBuffers[i].drawIndexed(mesh->GetIndexCount(), 1, 0, 0, 0);
            }
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

    CHECK(device.waitForFences(1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX));

    uint32_t imageIndex;
    vk::Result result = device.acquireNextImageKHR(swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == vk::Result::eErrorOutOfDateKHR) {
        RecreateSwapchain();
        return;
    } else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    UpdateUniformBuffer(imageIndex);

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

void VulkanCore::UpdateUniformBuffer(uint32_t currentImage) {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo;
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swapchainExtent.width / (float) swapchainExtent.height, 0.1f, 10.f);
    ubo.proj[1][1] *= -1;

    void* data;
    CHECK(device.mapMemory(uniformBuffersMemory[currentImage], 0, sizeof(ubo), (vk::MemoryMapFlags) 0, &data));
    memcpy(data, &ubo, sizeof(ubo));
    device.unmapMemory(uniformBuffersMemory[currentImage]);
}
void VulkanCore::CleanupSwapchain() {
    for(auto framebuffer : swapchainFramebuffers)
        device.destroyFramebuffer(framebuffer);

    device.freeCommandBuffers(commandPool, commandBuffers.size(), commandBuffers.data());

    device.destroyPipelineLayout(pipelineLayout);
    device.destroyRenderPass(renderPass);
    for (auto imageView : swapchainImageViews)
        device.destroyImageView(imageView);
    device.destroySwapchainKHR(swapchain);

    for(size_t i = 0; i < swapchainImages.size(); i++) {
        device.destroyBuffer(uniformBuffers[i]);
        device.freeMemory(uniformBuffersMemory[i]);
    }

    device.destroyDescriptorPool(descriptorPool);
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
    CreateDepthResources();
    CreateFramebuffers();
    CreateUniformBuffers();
    CreateDescriptorPool();
    CreateCommandBuffers();
    swapchainRecreated(this, width, height);
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
    staticMeshes.push_back(mesh);
    staticMeshAdded(this, mesh);
}

void VulkanCore::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer &buffer, vk::DeviceMemory &bufferMemory) {
    vk::BufferCreateInfo bufferInfo{};
    bufferInfo.setSize(size);
    bufferInfo.setUsage(usage);
    bufferInfo.setSharingMode(vk::SharingMode::eExclusive);

    CHECK(device.createBuffer(&bufferInfo, nullptr, &buffer));

    vk::MemoryRequirements memRequirements;
    device.getBufferMemoryRequirements(buffer, &memRequirements);

    // TODO Replace with GPUOPEN VMA?
    //   https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);
    CHECK(device.allocateMemory(&allocInfo, nullptr, &bufferMemory));

    device.bindBufferMemory(buffer, bufferMemory, 0);
}

void VulkanCore::CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size) {
    CommandBuffer copyBufferCmd(device, vk::CommandBufferLevel::ePrimary, commandPool);
    copyBufferCmd.Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    vk::BufferCopy copyRegion{};
    copyRegion.setSrcOffset(0).setDstOffset(0).setSize(size);
    ((vk::CommandBuffer) copyBufferCmd).copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

    copyBufferCmd.EndAndSubmit(graphicsQueue, VK_NULL_HANDLE, true);
}

void VulkanCore::DestroyBuffer(vk::Buffer &buffer, vk::DeviceMemory &bufferMemory) {
    device.destroyBuffer(buffer);
    device.freeMemory(bufferMemory);
}

void VulkanCore::CreateUniformBuffers() {
    vk::DeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers.resize(swapchainImages.size());
    uniformBuffersMemory.resize(swapchainImages.size());

    for(size_t i = 0; i < swapchainImages.size(); i++)
        CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                     uniformBuffers[i], uniformBuffersMemory[i]);
}

void VulkanCore::CreateDescriptorPool() {
    std::array<vk::DescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].setType(vk::DescriptorType::eUniformBuffer);
    poolSizes[0].setDescriptorCount(swapchainImages.size());
    poolSizes[1].setType(vk::DescriptorType::eCombinedImageSampler);
    poolSizes[1].setDescriptorCount(swapchainImages.size());

    vk::DescriptorPoolCreateInfo poolInfo{};
    poolInfo.setPoolSizes(poolSizes);
    poolInfo.setMaxSets(swapchainImages.size());

    CHECK(device.createDescriptorPool(&poolInfo, nullptr, &descriptorPool));
}
vk::DescriptorPool &VulkanCore::GetDescriptorPool() {
    return descriptorPool;
}
int VulkanCore::ImageCount() {
    return swapchainImages.size();
}
std::vector<vk::Buffer>& VulkanCore::GetUniformBuffers() {
    return uniformBuffers;
}

void VulkanCore::CreateDepthResources() {
    vk::Format depthFormat = FindDepthFormat();
    depthTexturePtr = CreateTexture(depthFormat, vk::ImageAspectFlagBits::eDepth);
    depthTexturePtr->usageFlags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
    depthTexturePtr->SetSize(swapchainExtent.width, swapchainExtent.height);
    depthTexturePtr->Create(swapchainExtent.width * swapchainExtent.height * 4);
//    TransitionImageLayout(depthTexturePtr->GetImage(), depthFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
}

vk::Format VulkanCore::FindSupportedFormat(const std::vector<vk::Format> &candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
    for (vk::Format format : candidates) {
        vk::FormatProperties props;
        physicalDevice.getFormatProperties(format, &props);

        if(tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features)
            return format;
        if(tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features)
            return format;
    }

    throw std::runtime_error("Failed to find supported format");
}

vk::Format VulkanCore::FindDepthFormat() {
    return FindSupportedFormat({vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
                               vk::ImageTiling::eOptimal,
                               vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

bool VulkanCore::HasStencilComponent(vk::Format format) {
    return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

void VulkanCore::BufferStaging(vk::DeviceSize size, vk::BufferUsageFlags destBufferFlags, vk::Buffer &buffer, vk::DeviceMemory &bufferMemory, std::function<void(void *)> func) {
    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;

    CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

    void* data;
    if(device.mapMemory(stagingBufferMemory, 0, size, (vk::MemoryMapFlags) 0, &data) != vk::Result::eSuccess)
        throw std::runtime_error("Unable to map device memory");
    func(data);
    device.unmapMemory(stagingBufferMemory);

    CreateBuffer(size, vk::BufferUsageFlagBits::eTransferDst | destBufferFlags, vk::MemoryPropertyFlagBits::eDeviceLocal, buffer, bufferMemory);
    CopyBuffer(stagingBuffer, buffer, size);
    DestroyBuffer(stagingBuffer, stagingBufferMemory);
}

void VulkanCore::TransitionImageLayout(vk::Image& image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
    CommandBuffer cmdBuffer(device, vk::CommandBufferLevel::ePrimary, commandPool);
    cmdBuffer.Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    vk::ImageMemoryBarrier barrier{};
    barrier.setOldLayout(oldLayout).setNewLayout(newLayout);
    barrier.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
    barrier.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
    barrier.setImage(image);
    barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
    barrier.subresourceRange.setBaseMipLevel(0);
    barrier.subresourceRange.setLevelCount(1);
    barrier.subresourceRange.setBaseArrayLayer(0);
    barrier.subresourceRange.setLayerCount(1);

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;
    if(oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eNoneKHR);
        barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    }else if(oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    }else if(oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eNoneKHR);
        barrier.setDstAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    }else{
        throw std::invalid_argument("Unsupported layout transition");
    }


    if(newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
        barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eDepth);
        if(HasStencilComponent(format))
            barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
    }else{
        barrier.subresourceRange.setAspectMask(vk::ImageAspectFlagBits::eColor);
    }

    cmdBuffer.Commands().pipelineBarrier(sourceStage,
                                         destinationStage,
                                         (vk::DependencyFlags) 0,
                                         0, nullptr,
                                         0, nullptr,
                                         1, &barrier);

    cmdBuffer.EndAndSubmit(graphicsQueue);
}

void VulkanCore::CopyBufferToImage(vk::Buffer &buffer, vk::Image &image, uint32_t width, uint32_t height) {
    CommandBuffer cmdBuffer(device, vk::CommandBufferLevel::ePrimary, commandPool);

    cmdBuffer.Begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

    vk::BufferImageCopy region{};
    region.setBufferOffset(0);
    region.setBufferRowLength(0);
    region.setBufferImageHeight(0);
    region.imageSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor);
    region.imageSubresource.setMipLevel(0);
    region.imageSubresource.setBaseArrayLayer(0);
    region.imageSubresource.setLayerCount(1);

    region.imageOffset = vk::Offset3D{0, 0, 0};
    region.imageExtent = vk::Extent3D{ width, height, 1 };

    cmdBuffer.Commands().copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, 1, &region);

    cmdBuffer.EndAndSubmit(graphicsQueue);
}

vk::ImageView VulkanCore::CreateImageView(vk::Image &image, vk::Format format, vk::ImageAspectFlags aspectFlags) {
    vk::ImageViewCreateInfo viewInfo{};
    viewInfo.setImage(image);
    viewInfo.setViewType(vk::ImageViewType::e2D);
    viewInfo.setFormat(format);
    viewInfo.subresourceRange.setAspectMask(aspectFlags);
    viewInfo.subresourceRange.setBaseMipLevel(0);
    viewInfo.subresourceRange.setLevelCount(1);
    viewInfo.subresourceRange.setBaseArrayLayer(0);
    viewInfo.subresourceRange.setLayerCount(1);

    vk::ImageView imageView;
    CHECK(device.createImageView(&viewInfo, nullptr, &imageView));
    return imageView;
}
vk::PhysicalDeviceProperties &VulkanCore::GetPhysicalDeviceProperties() {
    if(!hasDeviceProps) {
        deviceProps = physicalDevice.getProperties();
        hasDeviceProps = true;
    }
    return deviceProps;
}

vk::PhysicalDeviceFeatures &VulkanCore::GetPhysicalDeviceFeatures() {
    if(!hasDeviceFeatures) {
        deviceFeatures = physicalDevice.getFeatures();
        hasDeviceFeatures = true;
    }
    return deviceFeatures;
}

std::shared_ptr<ShaderProgram> VulkanCore::CreateShaderProgram() {
    return std::make_shared<ShaderProgram>(this, device);
}

void VulkanCore::CleanupCommandBuffers() {
    device.freeCommandBuffers(commandPool, commandBuffers.size(), commandBuffers.data());
}

void VulkanCore::RecreateStaticCommandBuffer() {
    CleanupCommandBuffers();
    CreateCommandBuffers();
}

void VulkanCore::CompileShader(const std::shared_ptr<ShaderProgram>& shaderPtr) {
    CompileShader(shaderPtr.get());
}
void VulkanCore::CompileShader(ShaderProgram* shaderPtr) {
    shaderPtr->Compile(swapchainExtent, swapchainImageFormat, descriptorPool);
}

std::shared_ptr<Texture> VulkanCore::CreateTexture(vk::Format format, vk::ImageAspectFlags aspectFlags) {
    return std::make_shared<Texture>(this, device, format, aspectFlags);
}
