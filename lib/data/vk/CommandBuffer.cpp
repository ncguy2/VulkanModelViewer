//
// Created by Guy on 12/06/2021.
//

#include <pch.h>

#include <data/vk/CommandBuffer.h>


CommandBuffer::CommandBuffer(vk::Device& device, vk::CommandBufferLevel level, vk::CommandPool& commandPool) : attachedDevice(&device), commandPool(&commandPool) {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.setLevel(level);
    allocInfo.setCommandPool(commandPool);
    allocInfo.setCommandBufferCount(1);

    CHECK(device.allocateCommandBuffers(&allocInfo, &commandBuffer));
}

CommandBuffer::~CommandBuffer() {
    attachedDevice->freeCommandBuffers(*commandPool, commandBuffer);
}

void CommandBuffer::Begin(vk::CommandBufferUsageFlags usage) {
    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.setFlags(usage);
    CHECK(commandBuffer.begin(&beginInfo));
}

void CommandBuffer::End() {
    commandBuffer.end();
}

vk::SubmitInfo CommandBuffer::Prepare() {
    vk::SubmitInfo submitInfo{};
    submitInfo.setCommandBufferCount(1);
    submitInfo.setPCommandBuffers(&commandBuffer);

    return submitInfo;
}

void CommandBuffer::Submit(vk::Queue queue, vk::Fence fence, bool waitIdle) {
    vk::SubmitInfo info = Prepare();
    CHECK(queue.submit(1, &info, fence));
    if(waitIdle)
        queue.waitIdle();
}

CommandBuffer::operator vk::CommandBuffer() {
    return commandBuffer;
}

void CommandBuffer::EndAndSubmit(vk::Queue queue, vk::Fence fence, bool waitIdle) {
    End();
    Submit(queue, fence, waitIdle);
}
vk::CommandBuffer& CommandBuffer::Commands() {
    return this->commandBuffer;
}
