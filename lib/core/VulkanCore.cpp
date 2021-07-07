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
#include <utility>

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <data/Mesh.h>
#include <data/vk/CommandBuffer.h>
#include <data/vk/Camera.h>
#include <core/Screen.h>
#include <data/render/MeshRenderer.h>

#define TEST(x) x != vk::Result::eSuccess
#define TEST_EXC(x, msg) if(x != vk::Result::eSuccess) throw std::runtime_error(msg);

#ifdef NDEBUG
#define IN_DEBUG(x)
const bool enableValidationLayers = true;
#else
#define IN_DEBUG(x) x,
const bool enableValidationLayers = true;
#endif

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

static void dropCallback(GLFWwindow* window, int droppedPathCount, const char** paths) {
    if(droppedPathCount <= 0)
        return;

    auto core = reinterpret_cast<VulkanCore*>(glfwGetWindowUserPointer(window));

    std::vector<FilePath> droppedPaths(droppedPathCount);

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;

    for(int i = 0; i < droppedPathCount; i++)
        droppedPaths[i] = convert.from_bytes(paths[i]);

    core->drop(droppedPaths);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto core = reinterpret_cast<VulkanCore*>(glfwGetWindowUserPointer(window));
    core->OnKey(key, scancode, action, mods);
}

static vk::Result CreateDebugUtilsMessengerEXT(vk::Instance instance, const vk::DebugUtilsMessengerCreateInfoEXT* pCreateInfo, const vk::AllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr((VkInstance) instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
        return vk::Result(func((VkInstance) instance, (VkDebugUtilsMessengerCreateInfoEXT*) pCreateInfo, (VkAllocationCallbacks*) pAllocator, (VkDebugUtilsMessengerEXT*) pDebugMessenger));
    return vk::Result(VK_ERROR_EXTENSION_NOT_PRESENT);
}

static vk::Result SetDebugUtilsObjectNameEXT(vk::Device device, const vk::DebugUtilsObjectNameInfoEXT* info) {
    static auto func = (PFN_vkSetDebugUtilsObjectNameEXT) vkGetDeviceProcAddr((VkDevice) device, "vkSetDebugUtilsObjectNameEXT");
    if (func != nullptr)
        return vk::Result(func((VkDevice) device, (VkDebugUtilsObjectNameInfoEXT*) info));
    return vk::Result(VK_ERROR_EXTENSION_NOT_PRESENT);
}

static void DestroyDebugUtilsMessengerEXT(vk::Instance instance, VkDebugUtilsMessengerEXT debugMessenger, const vk::AllocationCallbacks* pAllocator) {
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

    CHECK(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger));
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
    glfwSetDropCallback(window, dropCallback);
    glfwSetKeyCallback(window, keyCallback);

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
    CreateDepthResources();
    CreateRenderPass();
    CreateDescriptorPool();
    CreateUniformBuffers();
    CreateGraphicsPipeline();
    CreateCommandPool();
    CreateSyncObjects();

    pluginManager.Initialise(this);

    return window;
}

void VulkanCore::Cleanup() {

    if(currentScreen)
        currentScreen->Dispose();

    pluginManager.Dispose();

    if(enableValidationLayers)
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

    for(size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        device.destroySemaphore(renderFinishedSemaphores[i]);
        device.destroySemaphore(imageAvailableSemaphores[i]);
        device.destroyFence(inFlightFences[i]);
    }

    depthTexturePtr->Dispose();
    depthTexturePtr = nullptr;

    CleanupSwapchain();

    device.destroyCommandPool(commandPool);

    device.destroy();
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

    NameObject(swapchain, "Screen swapchain");

    swapchainImages = device.getSwapchainImagesKHR(swapchain);
    swapchainImageFormat = surfaceFormat.format;
    swapchainExtent = extent;

    for(int i = 0; i < swapchainImages.size(); i++) {
        NameObject(swapchainImages[i], "Swapchain image " + std::to_string(i));
    }

    swapchainTextures.resize(swapchainImages.size());
    for(int i = 0; i < swapchainImages.size(); i++) {
        swapchainTextures[i] = std::make_shared<Texture>(this, device, swapchainImageFormat, vk::ImageAspectFlagBits::eColor);
        swapchainTextures[i]->SetName("SwapchainTexture " + std::to_string(i));
        swapchainTextures[i]->Set(swapchainImages[i]);
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

void VulkanCore::CreateRenderPass() {
    swapchainRenderPass = std::make_unique<RenderPass>(&device);
    swapchainRenderPass->Resize(swapchainTextures.size());
    swapchainRenderPass->SetFBSize(swapchainExtent);
    unsigned int idx = 0;
    for (const auto &tex : swapchainTextures) {
        tex->usageFlags = vk::ImageUsageFlagBits::eColorAttachment;
        tex->format = swapchainImageFormat;
        swapchainRenderPass->AddFBAttachment(idx, tex);
        swapchainRenderPass->AddFBAttachment(idx, depthTexturePtr);
        idx++;
    }

    swapchainRenderPass->Build();
    NameObject(swapchainRenderPass->GetVK(), "Swapchain Render Pass");
}

void VulkanCore::CreateCommandPool(){
    QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physicalDevice);

    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.setQueueFamilyIndex(queueFamilyIndices.graphicsFamily.value());
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;

    if(device.createCommandPool(&poolInfo, nullptr, &commandPool) != vk::Result::eSuccess)
        throw std::runtime_error("Failed to create command pool");
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

    previousTime = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        DrawFrame();
    }

    device.waitIdle();
}

void VulkanCore::DrawFrame() {
    std::chrono::time_point<std::chrono::steady_clock> currentTime;
    currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - previousTime).count();
    previousTime = currentTime;

    EntityScene* scene;
    UpdateContext updateContext;

    updateContext.camera = nullptr;
    updateContext.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    updateContext.proj = glm::perspective(glm::radians(45.0f), swapchainExtent.width / (float) swapchainExtent.height, 0.1f, 10.0f);
    updateContext.proj[1][1] *= -1;

    if(currentScreen) {
        currentScreen->Update(deltaTime);
        scene = currentScreen->GetScenePtr();
        scene->Update(deltaTime, updateContext);
        SetViewProj(currentFrame, updateContext.view, updateContext.proj);
    }

    CHECK(device.waitForFences(1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX));

    RendererContext renderContext;
    renderContext.bufferIdx = currentFrame;
    renderContext.commandPool = &commandPool;
    renderContext.device = &device;
    renderContext.renderPass = swapchainRenderPass.get();
    renderContext.core = this;
    renderContext.extent = swapchainExtent;
    renderContext.data = updateContext;
    render(renderContext);

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

    // Dispatch command buffers to graphics queue
    if(updateContext.camera) {
        updateContext.camera->Render(device, graphicsQueue, imageAvailableSemaphores[currentFrame],
                                     renderFinishedSemaphores[currentFrame], inFlightFences[currentFrame], renderContext.commandBuffers);
    } else{
        vk::SubmitInfo submitInfo{};

        vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
        submitInfo.setWaitSemaphoreCount(1);
        submitInfo.setPWaitSemaphores(&imageAvailableSemaphores[currentFrame]);
        submitInfo.setPWaitDstStageMask(waitStages);

        submitInfo.setSignalSemaphoreCount(1);
        submitInfo.setPSignalSemaphores(&renderFinishedSemaphores[currentFrame]);

        submitInfo.setCommandBuffers(renderContext.commandBuffers);

        CHECK(device.resetFences(1, &inFlightFences[currentFrame]));
        CHECK(graphicsQueue.submit(1, &submitInfo, inFlightFences[currentFrame]));
    }

    // Present swapchain to screen
    vk::PresentInfoKHR presentInfo{};
    presentInfo.setWaitSemaphoreCount(1);
    presentInfo.setPWaitSemaphores(&renderFinishedSemaphores[currentFrame]);

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

void VulkanCore::SetViewProj(uint32_t currentImage, glm::mat4& view, glm::mat4& proj) {
    UniformBufferObject ubo{};
    ubo.view = view;
    ubo.proj = proj;

    void* data;
    CHECK(device.mapMemory(uniformBuffersMemory[currentImage], 0, sizeof(ubo), (vk::MemoryMapFlags) 0, &data));
    memcpy(data, &ubo, sizeof(ubo));
    device.unmapMemory(uniformBuffersMemory[currentImage]);
}

void VulkanCore::UpdateUniformBuffer(uint32_t currentImage) {
    UniformBufferObject ubo{};
//    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swapchainExtent.width / (float) swapchainExtent.height, 0.1f, 10.f);
    ubo.proj[1][1] *= -1;

    void* data;
    CHECK(device.mapMemory(uniformBuffersMemory[currentImage], 0, sizeof(ubo), (vk::MemoryMapFlags) 0, &data));
    memcpy(data, &ubo, sizeof(ubo));
    device.unmapMemory(uniformBuffersMemory[currentImage]);
}
void VulkanCore::CleanupSwapchain() {
    swapchainRenderPass->Dispose();


    for(int i = 0; i < swapchainTextures.size(); i++) {
        auto item = swapchainTextures[i];
        item->Dispose();
    }

    swapchainTextures.clear();
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
    CreateDepthResources();
    CreateRenderPass();
    CreateGraphicsPipeline();
    CreateUniformBuffers();
    CreateDescriptorPool();
    swapchainRecreated(this, width, height);
    currentScreen->Resize(width, height);
}

uint32_t VulkanCore::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

    for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if(typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    }
    throw std::runtime_error("Failed to find suitable memory type");
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
//    std::array<vk::DescriptorPoolSize, 2> poolSizes{};
//    poolSizes[0].setType(vk::DescriptorType::eUniformBuffer);
//    poolSizes[0].setDescriptorCount(swapchainImages.size() * 32);
//    poolSizes[1].setType(vk::DescriptorType::eCombinedImageSampler);
//    poolSizes[1].setDescriptorCount(swapchainImages.size() * 32);

    std::array<vk::DescriptorPoolSize, 1> poolSizes{};
    poolSizes[0].setType(vk::DescriptorType::eMutableVALVE);
    poolSizes[0].setDescriptorCount(16);

    vk::DescriptorPoolCreateInfo poolInfo{};
    poolInfo.setPoolSizes(poolSizes);
    poolInfo.setMaxSets(1024);
    poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;

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
//    vk::Format depthFormat = FindDepthFormat();
//    depthTexturePtr = CreateTexture(depthFormat, vk::ImageAspectFlagBits::eDepth);
//    depthTexturePtr->usageFlags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
//    depthTexturePtr->SetSize(swapchainExtent.width, swapchainExtent.height);
//    depthTexturePtr->Create(swapchainExtent.width * swapchainExtent.height * 4);
////    TransitionImageLayout(depthTexturePtr->GetImage(), depthFormat, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
    depthTexturePtr = CreateDepthTexture(swapchainExtent);
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
    return FindSupportedFormat({vk::Format::eD32Sfloat, /*vk::Format::eD32SfloatS8Uint,*/ vk::Format::eD24UnormS8Uint},
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

void VulkanCore::CopyBufferToImage(vk::Buffer &buffer, vk::Image &image, uint32_t width, uint32_t height, int offsetX, int offsetY) {
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

    region.imageOffset = vk::Offset3D{offsetX, offsetY, 0};
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

void VulkanCore::CompileShader(const std::shared_ptr<ShaderProgram>& shaderPtr) {
    CompileShader(shaderPtr.get());
}
void VulkanCore::CompileShader(ShaderProgram* shaderPtr) {
    shaderPtr->Compile(swapchainExtent, swapchainImageFormat, descriptorPool, swapchainRenderPass->GetVK());
}

std::shared_ptr<Texture> VulkanCore::CreateTexture(vk::Format format, vk::ImageAspectFlags aspectFlags) {
    return std::make_shared<Texture>(this, device, format, aspectFlags);
}

std::shared_ptr<Texture> VulkanCore::CreateDepthTexture(vk::Extent2D size) {
    static int depthTextureCounter = 0;
    vk::Format depthFormat = FindDepthFormat();
    std::shared_ptr<Texture> ptr = CreateTexture(depthFormat, vk::ImageAspectFlagBits::eDepth);
    ptr->SetName("Depth Texture " + std::to_string(depthTextureCounter++));
    ptr->usageFlags |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
    ptr->SetSize(size.width, size.height);
    ptr->Create(size.width * size.height * 4);
    return ptr;
}

void VulkanCore::OnKey(int key, int scancode, int action, int mods) {
    onKey(this, key, scancode, action, mods);
    if(currentScreen)
        currentScreen->OnKey(key, scancode, action, mods);
}

void VulkanCore::SetScreen(std::shared_ptr<Screen> newScreenPtr) {
    if(currentScreen) {
        currentScreen->Hide();
    }
    currentScreen = std::move(newScreenPtr);
    if(!currentScreen)
        return;

    CoreScreenComponents c = {
            this,
            &device,
            &commandPool,
            &this->pluginManager
    };
    currentScreen->AssignCoreComponents(c);
    currentScreen->Resize(swapchainExtent.width, swapchainExtent.height);

    if(!currentScreen->IsCreated())
        currentScreen->Create();

    currentScreen->Show();
}

vk::Device *VulkanCore::GetDevicePtr() {
    return &device;
}

void VulkanCore::NameObject(uint64_t handle, vk::ObjectType type, std::string name) {
    vk::DebugUtilsObjectNameInfoEXT nameInfo{};


    std::string a = (std::to_string(handle) + name);
    void* strPtr = malloc(a.size() * sizeof(char));
    memcpy(strPtr, a.data(), a.size());

    const char* str = reinterpret_cast<const char*>(strPtr);

    nameInfo.objectHandle = handle;
    nameInfo.objectType = type;
    nameInfo.pObjectName = str;

//    device.setDebugUtilsObjectNameEXT(nameInfo);
    SetDebugUtilsObjectNameEXT(device, &nameInfo);
    free(strPtr);
}
