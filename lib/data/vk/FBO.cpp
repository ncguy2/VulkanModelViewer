//
// Created by Guy on 18/06/2021.
//

#include <data/vk/FBO.h>
#include <pch.h>

bool FBOAttachment::Is(vk::ImageUsageFlags flag) const {
    return (usageFlags & flag) == flag;
}

vk::ImageLayout FBOAttachment::GetReferenceLayout() const {
    if(Is(vk::ImageUsageFlagBits::eDepthStencilAttachment))
        return vk::ImageLayout::eDepthStencilAttachmentOptimal;
    return vk::ImageLayout::eColorAttachmentOptimal;
}

vk::ImageLayout FBOAttachment::GetFinalLayout() const {
    if(Is(vk::ImageUsageFlagBits::eDepthStencilAttachment))
        return vk::ImageLayout::eDepthStencilAttachmentOptimal;
    return vk::ImageLayout::ePresentSrcKHR;
}


void FBO::Add(FBOAttachment &attachment) {
    attachments.push_back(attachment);
}

void FBO::SetSize(vk::Extent2D size) {
    this->size = size;

    if(isBufferValid)
        Rebuild();
}

void FBO::Build(vk::RenderPass &renderPass, vk::Device& device) {
    this->renderPass = &renderPass;
    this->device = &device;

    Rebuild();
}

void FBO::Rebuild() {
    if(isBufferValid)
        Dispose();

    std::vector<vk::ImageView> ivAttachments(attachments.size());
    for(int i = 0; i < attachments.size(); i++)
        ivAttachments[i] = *attachments[i].imageView;

    vk::FramebufferCreateInfo fboInfo{};
    fboInfo.setRenderPass(*renderPass);
    fboInfo.setAttachments(ivAttachments);
    fboInfo.setWidth(size.width).setHeight(size.height);
    fboInfo.setLayers(1);

    CHECK(device->createFramebuffer(&fboInfo, nullptr, &actualBuffer));

    isBufferValid = true;
}

int FBO::Count() {
    return attachments.size();
}
void FBO::Dispose() {
}
vk::Framebuffer& FBO::GetVK() {
    return actualBuffer;
}
std::vector<FBOAttachment>& FBO::GetAttachments() {
    return attachments;
}
