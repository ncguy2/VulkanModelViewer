//
// Created by Guy on 05/06/2021.
//

#ifndef GLMODELVIEWER_VULKANCORE_HPP
#define GLMODELVIEWER_VULKANCORE_HPP


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <chrono>
#include <core/Events.h>
#include <data/Contexts.h>
#include <data/RenderPass.h>
#include <data/Shader.h>
#include <glm/glm.hpp>
#include <optional>
#include <plugins/PluginManager.h>
#include <vulkan/vulkan.hpp>
#include <map>
#include <data/DataSmoother.h>

struct InputState {
    int key;
    bool pressed;
    // TODO implement
    bool justUpdated;
};

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
class Camera;
class Screen;
class MeshRenderer;

class VulkanCore {
public:

    typedef Delegate<VulkanCore*, uint32_t, uint32_t> SwapchainRecreated;
    typedef Delegate<VulkanCore*, std::shared_ptr<Mesh>> StaticMeshAdded;
    typedef Delegate<RendererContext&> Render;
    typedef Delegate<std::vector<FilePath>> Drop;

    // Event Delegates
    SwapchainRecreated swapchainRecreated;
    StaticMeshAdded staticMeshAdded;
    Render render;
    Drop drop;

    StaticMeshAdded::Signature staticMeshAddedHandle;

    std::shared_ptr<Camera> primaryCamera;

    vk::Device* GetDevicePtr();

    void NameObject(uint64_t handle, vk::ObjectType type, std::string name);

    template <typename T>
    void NameObject(T obj, std::string name) {
        NameObject(GET_VK_HANDLE(obj), obj.objectType, name);
    }

    void MouseButton(int button, int action);

public:
    GLFWwindow* InitVulkan(HWND handle = nullptr);
    void Cleanup();

    void MainLoop();

    bool framebufferResized = false;

    void CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory);

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
    void CopyBufferToImage(vk::Buffer& buffer, vk::Image& image, uint32_t width, uint32_t height, int offsetX = 0, int offsetY = 0);
    vk::ImageView CreateImageView(vk::Image& image, vk::Format format, vk::ImageAspectFlags aspectFlags);
    std::shared_ptr<ShaderProgram> CreateShaderProgram();
    void CompileShader(const std::shared_ptr<ShaderProgram>& shaderPtr);
    void CompileShader(ShaderProgram* shaderPtr);
    std::shared_ptr<Texture> CreateTexture(vk::Format format, vk::ImageAspectFlags aspectFlags);
    std::shared_ptr<Texture> CreateDepthTexture(vk::Extent2D size);

    void OnMouse(double xpos, double ypos);
    Delegate<VulkanCore*, double, double> onMouse;

    void OnKey(int key, int scancode, int action, int mods);
    Delegate<VulkanCore*, int, int, int, int> onKey;
    GLFWwindow* window;

    void SetScreen(std::shared_ptr<Screen> newScreenPtr);
    void SetViewProj(uint32_t currentImage, glm::mat4& view, glm::mat4& proj, glm::vec4& cameraData);
    vk::Format swapchainImageFormat;
    PluginManager pluginManager;
    glm::dvec2 mouseInput;

    void SetShouldClose() const;
    void NotifyDrop(std::vector<FilePath>& filesDropped);

protected:
    bool hasDrop = false;
    std::vector<FilePath> dropPayload;

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
    void CreateGraphicsPipeline();
    void CreateCommandPool();
    void CreateSyncObjects();
    void CleanupSwapchain();
    void RecreateSwapchain();
    void CreateUniformBuffers();
    void CreateDescriptorPool();
    void CreateDepthResources();

    void CreateRenderPass();

    void UpdateUniformBuffer(uint32_t currentImage);

    vk::Format FindSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
    vk::Format FindDepthFormat();
    bool HasStencilComponent(vk::Format format);

    void DrawFrame();

    vk::ShaderModule CreateShaderModule(const std::string& file);
    vk::ShaderModule CreateShaderModule(std::vector<char> source);

    vk::DispatchLoaderDynamic dynamicDispatch;
    vk::Instance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE;
    vk::Device device;
    vk::Queue graphicsQueue;
    vk::Queue presentQueue;

    vk::SurfaceKHR surface;

    vk::SwapchainKHR swapchain;
    std::vector<vk::Image> swapchainImages;
    std::vector<std::shared_ptr<Texture>> swapchainTextures;
    vk::Extent2D swapchainExtent;
    std::unique_ptr<RenderPass> swapchainRenderPass;

    vk::CommandPool commandPool;

    std::vector<vk::Semaphore> imageAvailableSemaphores;
    std::vector<vk::Semaphore> renderFinishedSemaphores;
    std::vector<vk::Fence> inFlightFences;
    std::vector<vk::Fence> imagesInFlight;
    size_t currentFrame = 0;

    std::shared_ptr<Screen> currentScreen;

    std::vector<vk::Buffer> uniformBuffers;
    std::vector<vk::DeviceMemory> uniformBuffersMemory;
    vk::DescriptorPool descriptorPool;

    std::shared_ptr<Texture> depthTexturePtr;

    vk::PhysicalDeviceProperties deviceProps;
    bool hasDeviceProps = false;
    vk::PhysicalDeviceFeatures deviceFeatures;
    bool hasDeviceFeatures = false;

    std::chrono::time_point<std::chrono::steady_clock> previousTime;

    std::map<int, InputState> keyInputs;
    std::map<int, InputState> buttonInputs;

    DataSmoother<unsigned int> fpsSmoother;

public:
    void Key(int key, int scancode, int action, int mods);

    bool IsKeyPressed(int key);
    bool IsButtonPressed(int button);

    void SetModelInfo(std::vector<std::string> lines);
    bool SetModelInfoFunc(void* funcPtr);

    void* modelInfoFuncPtr_raw = nullptr;
    typedef void* (*ModelInfoFunc)(int, int, const char*);
    ModelInfoFunc modelInfoFuncPtr = nullptr;
    std::vector<const char*> modelInfo;
    unsigned int GetFPS(bool smoothed);
};

#endif//GLMODELVIEWER_VULKANCORE_HPP
