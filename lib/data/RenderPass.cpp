//
// Created by Guy on 13/06/2021.
//

#include <data/RenderPass.h>

RenderPass::RenderPass(vk::Device *device) : VulkanDeviceObject(device) {}

void RenderPass::Build() {
    std::vector<FramebufferAttachment> attachments = framebuffer.GetAttachments();

    std::vector<vk::AttachmentDescription> attachmentDescriptions(framebuffer.Count(), vk::AttachmentDescription{});
    std::vector<vk::AttachmentReference> attachmentReferences(framebuffer.Count() - 1, vk::AttachmentReference{});
    vk::AttachmentReference depthAttachmentReference;

    int j = 0;
    for(int i = 0; i < attachments.size(); i++) {
        FramebufferAttachment attachment = attachments[i];

        attachmentDescriptions[i].setFormat(attachment.format);
        attachmentDescriptions[i].setSamples(vk::SampleCountFlagBits::e1);
        attachmentDescriptions[i].setLoadOp(vk::AttachmentLoadOp::eClear);
        attachmentDescriptions[i].setStoreOp(vk::AttachmentStoreOp::eStore);
        attachmentDescriptions[i].setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
        attachmentDescriptions[i].setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
        attachmentDescriptions[i].setInitialLayout(vk::ImageLayout::eUndefined);
        attachmentDescriptions[i].setFinalLayout(attachment.GetFinalLayout());

        if(attachment.Is(vk::ImageUsageFlagBits::eDepthStencilAttachment)) {
            depthAttachmentReference.setAttachment(i);
            depthAttachmentReference.setLayout(attachment.GetReferenceLayout());
        }else{
            attachmentReferences[j].setAttachment(i);
            attachmentReferences[j].setLayout(attachment.GetReferenceLayout());
            j++;
        }
    }

    vk::SubpassDescription subpass{};
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
    subpass.setColorAttachments(attachmentReferences);
    subpass.setPDepthStencilAttachment(&depthAttachmentReference);

    vk::SubpassDependency dependencies[2];
    dependencies[0].setSrcSubpass(VK_SUBPASS_EXTERNAL);
    dependencies[0].setDstSubpass(0);
    dependencies[0].setSrcStageMask(vk::PipelineStageFlagBits::eBottomOfPipe);
    dependencies[0].setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    dependencies[0].setSrcAccessMask(vk::AccessFlagBits::eMemoryRead);
    dependencies[0].setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);

    dependencies[1].setSrcSubpass(0);
    dependencies[1].setDstSubpass(VK_SUBPASS_EXTERNAL);
    dependencies[1].setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    dependencies[1].setDstStageMask(vk::PipelineStageFlagBits::eBottomOfPipe);
    dependencies[1].setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);
    dependencies[1].setDstAccessMask(vk::AccessFlagBits::eMemoryRead);

    vk::RenderPassCreateInfo renderPassInfo{};
    renderPassInfo.setAttachments(attachmentDescriptions);
    renderPassInfo.setSubpassCount(1);
    renderPassInfo.setPSubpasses(&subpass);
    renderPassInfo.setDependencyCount(2);
    renderPassInfo.setPDependencies(dependencies);

    if(device->createRenderPass(&renderPassInfo, nullptr, &vkRenderPass) != vk::Result::eSuccess)
        throw std::runtime_error("Failed to create render pass");

    BuildFramebuffer();
}

vk::RenderPass& RenderPass::GetVK() {
    return vkRenderPass;
}
void RenderPass::BuildFramebuffer() {
    framebuffer.Build(this);
}

void RenderPass::AddFBAttachment(std::shared_ptr<Texture> texture) {
    framebuffer.AddAttachment(texture, texture->usageFlags, texture->format);
}

void RenderPass::SetFBSize(vk::Extent2D size) {
    framebuffer.SetSize(size);
}
void RenderPass::Dispose() {
    framebuffer.Dispose();
}

SwapchainRenderPass::SwapchainRenderPass(vk::Device *device) : RenderPass(device) {}
void SwapchainRenderPass::BuildFramebuffer() {
    RenderPass::BuildFramebuffer();
    for (auto &item : additionalFramebuffers)
        item.Build(this);
}

void SwapchainRenderPass::AddFBAttachment(std::shared_ptr<Texture> texture) {
    RenderPass::AddFBAttachment(texture);
    for (auto &item : additionalFramebuffers)
        item.AddAttachment(texture, texture->usageFlags, texture->format);
}
void SwapchainRenderPass::SetFBSize(vk::Extent2D size) {
    RenderPass::SetFBSize(size);
    for (auto &item : additionalFramebuffers)
        item.SetSize(size);
}
int SwapchainRenderPass::Count() {
    return additionalFramebuffers.size() + 1;
}

void SwapchainRenderPass::SetFBCount(int amount) {
    additionalFramebuffers.resize(amount - 1);
}

void SwapchainRenderPass::AddFBAttachments(std::vector<std::shared_ptr<Texture>> textures, vk::ImageUsageFlags usage, vk::Format format) {
    framebuffer.AddAttachment(textures[0], usage, format);
    for(int i = 0; i < additionalFramebuffers.size(); i++) {
        additionalFramebuffers[i].AddAttachment(textures[i + 1], usage, format);
    }
}
void SwapchainRenderPass::Dispose() {
    RenderPass::Dispose();
    for (auto &item : additionalFramebuffers)
        item.Dispose();
}
