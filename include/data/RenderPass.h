//
// Created by Guy on 13/06/2021.
//

#ifndef GLMODELVIEWER_RENDERPASS_H
#define GLMODELVIEWER_RENDERPASS_H

#include <pch.h>
#include "Framebuffer.h"
#include <core/VulkanDeviceObject.h>
#include <vulkan/vulkan.hpp>

class RenderPass : public VulkanDeviceObject {
public:
    explicit RenderPass(vk::Device *device);
    void Build();

    virtual void AddFBAttachment(unsigned int fboIdx, vk::ImageView* texture, vk::ImageUsageFlags usage, vk::Format format);
    virtual void AddFBAttachment(unsigned int fboIdx, std::shared_ptr<Texture> texture);
    virtual void SetFBSize(vk::Extent2D size);
    virtual void Dispose();

    vk::RenderPass& GetVK();

    vk::Format format;
    Framebuffer& Get(unsigned int i) {
        return framebuffers[i];
    }

    Framebuffer& operator[](int i) {
        return Get(i);
    }

    void Resize(unsigned int amt);
    unsigned int Count();

    /**
     *
     * @param core Pointer to the VulkanCore instance
     * @param size The extents of the framebuffer/s
     * @param textures A vector returning the colour textures created
     * @param depthTextureList A vector returning the depth textures created
     * @param bufferCount Amount of framebuffers to create as part of this renderpass
     * @return
     */
    static std::shared_ptr<RenderPass> CreateStandardColourDepthPass(VulkanCore* core, vk::Extent2D size, std::vector<std::shared_ptr<Texture>>& textures, std::vector<std::shared_ptr<Texture>>& depthTextureList, int bufferCount = MAX_FRAMES_IN_FLIGHT);

protected:
    virtual void BuildFramebuffer();

    std::vector<Framebuffer> framebuffers;
    vk::RenderPass vkRenderPass;
};

#endif//GLMODELVIEWER_RENDERPASS_H
