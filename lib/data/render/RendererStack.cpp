//
// Created by Guy on 07/07/2021.
//

#include <pch.h>
#include <data/render/RendererStack.h>
#include <data/render/AbstractRenderer.h>
#include <iostream>
#include <Logging.h>


void RendererStack::Setup(RendererSetupContext &context) {

    setupContext = context;

    // Allocate command buffers up-front
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.commandPool = *context.commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    CHECK(context.device->allocateCommandBuffers(&allocInfo, commandBuffers.data()));

    for (auto &item : stack)
        item->Setup(context);
}

void RendererStack::Render(RendererContext &context) {

    vk::CommandBuffer buffer = commandBuffers[context.bufferIdx];

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

    CHECK(buffer.begin(&beginInfo));

    vk::RenderPassBeginInfo renderPassInfo{};
    renderPassInfo.setRenderPass(context.renderPass->GetVK());
    renderPassInfo.setFramebuffer(context.fbo);
    renderPassInfo.renderArea.setOffset({0, 0});
    renderPassInfo.renderArea.setExtent(context.extent);
    std::array<vk::ClearValue, 2> clearValues{};
    clearValues[0].color.setFloat32({0, 0, 0, 1});
    clearValues[1].depthStencil.setDepth(1.0f).setStencil(0);
    renderPassInfo.setClearValues(clearValues);

    buffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

    context.buffer = buffer;

    int idx = 0;
    for (auto &item : stack) {
        if(IsEnabled(idx))
            item->Render(context);
        idx++;
    }

    buffer.endRenderPass();
    buffer.end();

    context.commandBuffers.push_back(buffer);
}

void RendererStack::Dispose() {
    setupContext.device->freeCommandBuffers(*setupContext.commandPool, commandBuffers);

    for (auto &item : stack)
        item->Dispose();

    stack.clear();
}

void RendererStack::Resize(int width, int height) {
    for (auto &item : stack)
        item->Resize(width, height);
}

bool RendererStack::IsEnabled(int slot) {
    return ((disabledMask >> slot) & 1) == 0;
}

void RendererStack::Toggle(int slot) {
//    disabledMask ^= (-(unsigned long) slot ^ disabledMask) & (1UL << slot);

    if(IsEnabled(slot)) {
        disabledMask |= 1 << slot;
    }else{
        disabledMask &= ~(1 << slot);
    }

    if(stack.size() <= slot)
        return;

//    printf("%s %s\n", IsEnabled(slot) ? "Enabled" : "Disabled", stack[slot]->Name());
    Logging::Get() << (IsEnabled(slot) ? "Enabled" : "Disabled") << " " << stack[slot]->Name() << std::endl;
}
