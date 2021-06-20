//
// Created by Guy on 13/06/2021.
//

#include <pch.h>

#include <data/Framebuffer.h>
#include <data/RenderPass.h>

#include <utility>

void Framebuffer::AddAttachment(vk::ImageView* texture, vk::ImageUsageFlags usage, vk::Format format) {
    attachments.push_back(FramebufferAttachment{texture, usage, format});
}

void Framebuffer::Build(RenderPass *renderPass) {
    SetDevice(renderPass->device);

    std::vector<vk::ImageView> ivAttachments(attachments.size());
    for(int i = 0; i < attachments.size(); i++)
        ivAttachments[i] = *attachments[i].texture;

    vk::FramebufferCreateInfo framebufferInfo{};
    framebufferInfo.setRenderPass(renderPass->GetVK());
    framebufferInfo.setAttachments(ivAttachments);
    framebufferInfo.setWidth(size.width).setHeight(size.height);
    framebufferInfo.setLayers(1);

    CHECK(device->createFramebuffer(&framebufferInfo, nullptr, &vkFramebuffer));
}

void Framebuffer::SetSize(vk::Extent2D size) {
    this->size = size;
}

void Framebuffer::Dispose() {
    device->destroyFramebuffer(vkFramebuffer);
}

std::vector<FramebufferAttachment> Framebuffer::GetAttachments() {
    return attachments;
}

int Framebuffer::Count() {
    return attachments.size();
}
vk::Framebuffer Framebuffer::GetVK() {
    return vkFramebuffer;
}
void Framebuffer::SetDevice(vk::Device *device) {
    this->device = device;
}

bool FramebufferAttachment::Is(vk::ImageUsageFlags mask) const {
    return (usageFlags & mask) == mask;
}

vk::ImageLayout FramebufferAttachment::GetReferenceLayout() {
    if(Is(vk::ImageUsageFlagBits::eDepthStencilAttachment))
        return vk::ImageLayout::eDepthStencilAttachmentOptimal;
    return vk::ImageLayout::eColorAttachmentOptimal;
}

vk::ImageLayout FramebufferAttachment::GetFinalLayout() {
    if (Is(vk::ImageUsageFlagBits::eDepthStencilAttachment))
        return vk::ImageLayout::eDepthStencilAttachmentOptimal;
    return vk::ImageLayout::ePresentSrcKHR;
}