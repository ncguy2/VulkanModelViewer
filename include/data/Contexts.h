//
// Created by Guy on 20/06/2021.
//

#ifndef GLMODELVIEWER_CONTEXTS_H
#define GLMODELVIEWER_CONTEXTS_H

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

class Mesh;
class VulkanCore;
class RenderPass;
class Camera;

struct UpdateContext {
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec4 cameraData;
    std::vector<std::shared_ptr<Mesh>> meshesToRender;
    VulkanCore* core{};
    Camera* camera;
    glm::dvec2 mouseInput;
    float delta;

    void PushMesh(std::shared_ptr<Mesh> mesh);
    void Reset();
    bool GetKey(int keycode);
    bool GetButton(int button);
};

struct RendererSetupContext {
    VulkanCore* core{};
    vk::Device* device{};
    vk::CommandPool* commandPool{};
    std::vector<vk::ImageView> textureSet;
};

struct RendererContext {
    RenderPass* renderPass{};
    VulkanCore* core{};
    vk::Device* device{};
    vk::CommandPool* commandPool{};
    unsigned int bufferIdx{};
    vk::Extent2D extent{};
    UpdateContext data;

    vk::CommandBuffer buffer;
    std::vector<vk::CommandBuffer> commandBuffers;

    // TODO TEMP
    vk::Framebuffer fbo;
};

#endif//GLMODELVIEWER_CONTEXTS_H
