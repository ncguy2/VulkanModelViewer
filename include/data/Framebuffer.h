//
// Created by Guy on 13/06/2021.
//

#ifndef GLMODELVIEWER_FRAMEBUFFER_H
#define GLMODELVIEWER_FRAMEBUFFER_H

#include <core/VulkanDeviceObject.h>
#include <data/Texture.h>
#include <vulkan/vulkan.hpp>

struct FramebufferAttachment {
    vk::ImageView* texture;
    vk::ImageUsageFlags usageFlags;
    vk::Format format;


    bool Is(vk::ImageUsageFlags flag) const;
    vk::ImageLayout GetReferenceLayout();
    vk::ImageLayout GetFinalLayout();
};

class RenderPass;

class Framebuffer {
public:

    void SetDevice(vk::Device* device);

    void AddAttachment(vk::ImageView* texture, vk::ImageUsageFlags usage, vk::Format format);
    void SetSize(vk::Extent2D size);
    void Build(RenderPass* renderPass);

    std::vector<FramebufferAttachment> GetAttachments();

    void Dispose();
    int Count();

    vk::Framebuffer GetVK();

protected:
    vk::Device *device;
    vk::Extent2D size;
    vk::Framebuffer vkFramebuffer;
    std::vector<FramebufferAttachment> attachments;
};

#endif//GLMODELVIEWER_FRAMEBUFFER_H
