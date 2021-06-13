//
// Created by Guy on 05/06/2021.
//

#ifndef GLMODELVIEWER_VULKANCORE_HPP
#define GLMODELVIEWER_VULKANCORE_HPP

#include <core/Events.h>
#include <data/Shader.h>
#include <optional>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

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

    typedef Delegate<VulkanCore*, uint32_t, uint32_t> SwapchainRecreated;
    typedef Delegate<VulkanCore*, std::shared_ptr<Mesh>> StaticMeshAdded;

    // Event Delegates
    SwapchainRecreated swapchainRecreated;
    StaticMeshAdded staticMeshAdded;

    StaticMeshAdded::Signature staticMeshAddedHandle;

public:
    GLFWwindow* InitVulkan();
    void Cleanup();

    void MainLoop();

    bool framebufferResized = false;

    void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory);
    void AddMesh(std::shared_ptr<Mesh> mesh);

    void CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
    void DestroyBuffer(vk::Buffer& buffer, vk::DeviceMemory& bufferMemory);

    void BufferStaging(vk::DeviceSize size, vk::BufferUsageFlags destBufferFlags, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory, std::function<void(void*)> func);

    vk::DescriptorPool& GetDescriptorPool();
    vk::PhysicalDeviceProperties& GetPhysicalDeviceProperties();
    vk::PhysicalDeviceFeatures& GetPhysicalDeviceFeatures();

    int ImageCount();

    std::vector<vk::Buffer>& GetUniformBuffers();

    uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

    void TransitionImageLayout(vk::Image& image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
    void CopyBufferToImage(vk::Buffer& buffer, vk::Image& image, uint32_t width, uint32_t height);
    vk::ImageView CreateImageView(vk::Image& image, vk::Format format, vk::ImageAspectFlags aspectFlags);
    std::shared_ptr<ShaderProgram> CreateShaderProgram();
    void CompileShader(const std::shared_ptr<ShaderProgram>& shaderPtr);
    void CompileShader(ShaderProgram* shaderPtr);
    std::shared_ptr<Texture> CreateTexture(vk::Format format, vk::ImageAspectFlags aspectFlags);

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
    void CreateUniformBuffers();
    void CreateDescriptorPool();
    void CreateDepthResources();
    void CleanupCommandBuffers();
    void RecreateStaticCommandBuffer();

    void UpdateUniformBuffer(uint32_t currentImage);

    vk::Format FindSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
    vk::Format FindDepthFormat();
    bool HasStencilComponent(vk::Format format);

    void DrawFrame();

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

    std::vector<vk::Framebuffer> swapchainFramebuffers;

    vk::CommandPool commandPool;
    std::vector<vk::CommandBuffer> commandBuffers;

    std::vector<vk::Semaphore> imageAvailableSemaphores;
    std::vector<vk::Semaphore> renderFinishedSemaphores;
    std::vector<vk::Fence> inFlightFences;
    std::vector<vk::Fence> imagesInFlight;
    size_t currentFrame = 0;

    GLFWwindow* window;
    std::vector<std::shared_ptr<Mesh>> staticMeshes;

//    std::shared_ptr<ShaderProgram> defaultShaderProgram;

    std::vector<vk::Buffer> uniformBuffers;
    std::vector<vk::DeviceMemory> uniformBuffersMemory;
    vk::DescriptorPool descriptorPool;

    std::shared_ptr<Texture> depthTexturePtr;

    vk::PhysicalDeviceProperties deviceProps;
    bool hasDeviceProps = false;
    vk::PhysicalDeviceFeatures deviceFeatures;
    bool hasDeviceFeatures = false;

};

#endif//GLMODELVIEWER_VULKANCORE_HPP
