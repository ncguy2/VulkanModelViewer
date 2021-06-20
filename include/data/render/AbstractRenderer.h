//
// Created by Guy on 20/06/2021.
//

#ifndef GLMODELVIEWER_ABSTRACTRENDERER_H
#define GLMODELVIEWER_ABSTRACTRENDERER_H

#include <array>
#include <core/VulkanCore.hpp>
#include <pch.h>
#include <vulkan/vulkan.hpp>

#include <data/Contexts.h>

class Mesh;

class AbstractRenderer {
public:
    virtual void Setup(RendererSetupContext& context);
    virtual void Render(RendererContext& context) = 0;

    vk::CommandBuffer GetCommandBuffer(unsigned int idx);
    vk::CommandBuffer* GetCommandBufferPtr(unsigned int idx);

protected:
    std::array<vk::CommandBuffer, MAX_FRAMES_IN_FLIGHT> commandBuffers;
};

#endif//GLMODELVIEWER_ABSTRACTRENDERER_H
