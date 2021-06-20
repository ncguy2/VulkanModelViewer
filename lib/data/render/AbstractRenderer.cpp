//
// Created by Guy on 20/06/2021.
//

#include <data/render/AbstractRenderer.h>

void AbstractRenderer::Setup(RendererSetupContext& context) {
    // Allocate command buffers up-front
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = *context.commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    CHECK(context.device->allocateCommandBuffers(&allocInfo, commandBuffers.data()));
}

vk::CommandBuffer AbstractRenderer::GetCommandBuffer(unsigned int idx) {
    return commandBuffers[idx & MAX_FRAMES_IN_FLIGHT];
}

vk::CommandBuffer *AbstractRenderer::GetCommandBufferPtr(unsigned int idx) {
    return &commandBuffers[idx % MAX_FRAMES_IN_FLIGHT];
}
