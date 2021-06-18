//
// Created by Guy on 13/06/2021.
//

#ifndef GLMODELVIEWER_RENDERPASS_H
#define GLMODELVIEWER_RENDERPASS_H

#include "Framebuffer.h"
#include <core/VulkanDeviceObject.h>
#include <vulkan/vulkan.hpp>

class RenderPass : public VulkanDeviceObject {
public:
    explicit RenderPass(vk::Device *device);
    void Build();

    virtual void AddFBAttachment(std::shared_ptr<Texture> texture);
    virtual void SetFBSize(vk::Extent2D size);
    virtual void Dispose();

    vk::RenderPass& GetVK();

    vk::Format format;
protected:
    virtual void BuildFramebuffer();

    Framebuffer framebuffer;
    vk::RenderPass vkRenderPass;
};

class SwapchainRenderPass : public RenderPass {
public:
    explicit SwapchainRenderPass(vk::Device *device);
    void SetFBCount(int amount);
    void AddFBAttachment(std::shared_ptr<Texture> texture) override;
    void SetFBSize(vk::Extent2D size) override;
    void Dispose() override;

    void AddFBAttachments(std::vector<std::shared_ptr<Texture>> textures, vk::ImageUsageFlags usage, vk::Format format);


    Framebuffer& Get(int i) {
        if(i == 0)
            return framebuffer;

        i -= 1;

        if(i >= additionalFramebuffers.size())
            throw std::invalid_argument("Index too high");

        return additionalFramebuffers[i];
    }

    Framebuffer& operator[](int i) {
        return Get(i);
    }

    int Count();

protected:
    void BuildFramebuffer() override;
    std::vector<Framebuffer> additionalFramebuffers;
};

#endif//GLMODELVIEWER_RENDERPASS_H
