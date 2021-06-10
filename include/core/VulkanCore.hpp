//
// Created by Guy on 05/06/2021.
//

#ifndef GLMODELVIEWER_VULKANCORE_HPP
#define GLMODELVIEWER_VULKANCORE_HPP

#include <vulkan/vulkan.hpp>
#include <optional>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool IsComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

class GLFWwindow;
class Mesh;

class VulkanCore {
public:
    GLFWwindow* InitVulkan();
    void Cleanup();

    void MainLoop();

    bool framebufferResized = false;

    void AllocateMemory(vk::Buffer& buffer, vk::DeviceMemory* memory);
    void AddMesh(std::shared_ptr<Mesh> mesh);

protected:
    void CreateInstance();
    std::vector<const char*> getRequiredExtensions();
    bool checkValidationLayerSupport();
    void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo);
    void SetupDebugMessenger();
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    int RateDeviceSuitability(vk::PhysicalDevice device);
    QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device);
    void CreateSurface();
    bool CheckDeviceExtensionSupport(vk::PhysicalDevice device);
    SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device);
    vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
    vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
    void CreateSwapChain();
    void CreateImageViews();
    void CreateGraphicsPipeline();
    void CreateRenderPass();
    void CreateFramebuffers();
    void CreateCommandPool();
    void CreateCommandBuffers();
    void CreateSyncObjects();
    void CleanupSwapchain();
    void RecreateSwapchain();

    void DrawFrame();

    uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    vk::ShaderModule CreateShaderModule(const std::string& file);
    vk::ShaderModule CreateShaderModule(std::vector<char> source);

    vk::Instance instance;
    vk::DebugUtilsMessengerEXT debugMessenger;
    vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;
    vk::Device device;
    vk::Queue graphicsQueue;
    vk::Queue presentQueue;

    vk::SurfaceKHR surface;

    vk::SwapchainKHR swapchain;
    std::vector<vk::Image> swapchainImages;
    vk::Format swapchainImageFormat;
    vk::Extent2D swapchainExtent;
    std::vector<vk::ImageView> swapchainImageViews;

    vk::RenderPass renderPass;
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline graphicsPipeline;

    std::vector<vk::Framebuffer> swapchainFramebuffers;

    vk::CommandPool commandPool;
    std::vector<vk::CommandBuffer> commandBuffers;

    std::vector<vk::Semaphore> imageAvailableSemaphores;
    std::vector<vk::Semaphore> renderFinishedSemaphores;
    std::vector<vk::Fence> inFlightFences;
    std::vector<vk::Fence> imagesInFlight;
    size_t currentFrame = 0;

    GLFWwindow* window;
    std::vector<std::shared_ptr<Mesh>> meshes;
};

#endif//GLMODELVIEWER_VULKANCORE_HPP
