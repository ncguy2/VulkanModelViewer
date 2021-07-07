//
// Created by Guy on 13/06/2021.
//

#include <data/RenderPass.h>
#include <core/VulkanCore.hpp>

RenderPass::RenderPass(vk::Device *device) : VulkanDeviceObject(device) {}

void RenderPass::Build() {
    std::vector<FramebufferAttachment> attachments = framebuffers[0].GetAttachments();


    std::vector<vk::AttachmentDescription> attachmentDescriptions(framebuffers[0].Count(), vk::AttachmentDescription{});
    std::vector<vk::AttachmentReference> attachmentReferences(framebuffers[0].Count() - 1, vk::AttachmentReference{});
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
    for (auto &item : framebuffers)
        item.Build(this);
}

void RenderPass::AddFBAttachment(unsigned int fboIdx, vk::ImageView* texture, vk::ImageUsageFlags usage, vk::Format format) {
    framebuffers[fboIdx].AddAttachment(texture, usage, format);
}

void RenderPass::AddFBAttachment(unsigned int fboIdx, std::shared_ptr<Texture> texture) {
    AddFBAttachment(fboIdx, texture->GetViewPtr(), texture->usageFlags, texture->format);
}

void RenderPass::SetFBSize(vk::Extent2D size) {
    for (auto &item : framebuffers)
        item.SetSize(size);
}
void RenderPass::Dispose() {
    for (auto &item : framebuffers)
        item.Dispose();

    device->destroyRenderPass(vkRenderPass);
}

unsigned int RenderPass::Count() {
    return framebuffers.size();
}
void RenderPass::Resize(unsigned int amt) {
    framebuffers.resize(amt);
}

std::shared_ptr<RenderPass> RenderPass::CreateStandardColourDepthPass(VulkanCore* core, vk::Extent2D size, std::vector<std::shared_ptr<Texture>>& textureList, std::vector<std::shared_ptr<Texture>>& depthTextureList, int bufferCount) {
    static int RenderPassCounter = 0;
    std::shared_ptr<RenderPass> pass = std::make_shared<RenderPass>(core->GetDevicePtr());
    pass->Resize(bufferCount);

    int texSize = size.width * size.height * 4;

    for(int i = 0; i < bufferCount; i++) {
        std::shared_ptr<Texture> texture = core->CreateTexture(core->swapchainImageFormat, vk::ImageAspectFlagBits::eColor);
        texture->usageFlags |= vk::ImageUsageFlagBits::eColorAttachment;
        texture->SetSize(size.width, size.height);
        texture->Create(texSize);
        std::shared_ptr<Texture> depthTexture = core->CreateDepthTexture(size);
        textureList[i] = texture;
        depthTextureList[i] = depthTexture;
        pass->AddFBAttachment(i, texture);
        pass->AddFBAttachment(i, depthTexture);
    }

    pass->SetFBSize(size);

    pass->Build();

    core->NameObject(pass->GetVK(), "Render Pass " + std::to_string(RenderPassCounter));

    return pass;
}
