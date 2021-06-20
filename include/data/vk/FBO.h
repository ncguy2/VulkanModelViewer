//
// Created by Guy on 18/06/2021.
//

#ifndef GLMODELVIEWER_FBO_H
#define GLMODELVIEWER_FBO_H

#include <vulkan/vulkan.hpp>

struct FBOAttachment {
    std::shared_ptr<vk::ImageView> imageView{};
    vk::ImageUsageFlags usageFlags;
    vk::Format format;

    [[nodiscard]] bool Is(vk::ImageUsageFlags flag) const;
    [[nodiscard]] vk::ImageLayout GetReferenceLayout() const;
    [[nodiscard]] vk::ImageLayout GetFinalLayout() const;
};

class FBO {
public:
    void Add(FBOAttachment& attachment);
    void SetSize(vk::Extent2D size);
    void Build(vk::RenderPass& renderPass, vk::Device& device);
    int Count();
    void Dispose();
    vk::Framebuffer& GetVK();
    std::vector<FBOAttachment>& GetAttachments();

    void Rebuild();

protected:
    bool isBufferValid = false;
    vk::Framebuffer actualBuffer;
    vk::Extent2D size;
    vk::Device* device;
    vk::RenderPass* renderPass;
    std::vector<FBOAttachment> attachments;
};

#endif//GLMODELVIEWER_FBO_H
