//
// Created by Guy on 03/06/2021.
//

#ifndef GLMODELVIEWER_SHADER_H
#define GLMODELVIEWER_SHADER_H

#include <string>
#include <vector>

#include "ShaderStage.h"
#include <vulkan/vulkan.hpp>


class AbstractShaderProgram {
};

class ShaderProgram : public AbstractShaderProgram {
public:
    ShaderProgram(vk::Device *device);
    void Compile(vk::Extent2D swapchainExtent, vk::Format imageFormat);
    void Cleanup();

    void AddStage(std::shared_ptr<ShaderStage> stage);

protected:
    void BuildRenderPass(vk::Format imageFormat);
    void BuildPipeline(vk::Extent2D extent);

    vk::Device* device;
    std::vector<std::shared_ptr<ShaderStage>> shaderStages;
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline pipeline;
    vk::RenderPass renderPass;

};

#endif//GLMODELVIEWER_SHADER_H
