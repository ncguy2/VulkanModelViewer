//
// Created by Guy on 12/06/2021.
//

#ifndef GLMODELVIEWER_COMMANDBUFFER_H
#define GLMODELVIEWER_COMMANDBUFFER_H

#include <vulkan/vulkan.hpp>

class CommandBuffer {
public:
    CommandBuffer(vk::Device& device, vk::CommandBufferLevel level, vk::CommandPool& commandPool);
    ~CommandBuffer();

    /**
     * Begin recording into the command buffer
     * @param usage The usage flags for this command buffer
     */
    void Begin(vk::CommandBufferUsageFlags usage);

    /**
     * Finishes recording into the command buffer
     */
    void End();

    /**
     * Prepares this command buffer for submission
     * @return The SubmitInfo struct for this buffer
     */
    vk::SubmitInfo Prepare();

    /**
     * Submits this command buffer for execution to the given queue
     * @param queue The queue that should execute this command buffer
     * @param fence The fence to signal when work is finished [default: VK_NULL_HANDLE]
     * @param waitIdle Whether se should wait for the queue to be idle before continuing [default: true]
     */
    void Submit(vk::Queue queue, vk::Fence fence = VK_NULL_HANDLE, bool waitIdle = true);

    /**
     * Finishes recording and submits this command buffer for execution to the given queue
     * Helper function joining the End and Submit functions
     * @param queue The queue that should execute this command buffer
     * @param fence The fence to signal when work is finished [default: VK_NULL_HANDLE]
     * @param waitIdle Whether se should wait for the queue to be idle before continuing [default: true]
     */
    void EndAndSubmit(vk::Queue queue, vk::Fence fence = VK_NULL_HANDLE, bool waitIdle = true);

    operator vk::CommandBuffer();

    vk::CommandBuffer& Commands();

protected:
    vk::Device* attachedDevice;
    vk::CommandPool* commandPool;
    vk::CommandBuffer commandBuffer;
};

#endif//GLMODELVIEWER_COMMANDBUFFER_H
