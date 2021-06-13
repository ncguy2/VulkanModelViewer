//
// Created by Guy on 03/06/2021.
//

#ifndef GLMODELVIEWER_SHADER_H
#define GLMODELVIEWER_SHADER_H

#include <core/Events.h>
#include <string>
#include <vector>

#include "ShaderStage.h"
#include "Texture.h"
#include "TextureSampler.h"
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class VulkanCore;

class AbstractShaderProgram {
};

class ShaderProgram : public AbstractShaderProgram {
public:

    typedef Delegate<ShaderProgram*, TextureSampler> SamplerAdded;
    typedef Delegate<ShaderProgram*, vk::ImageView, int> TextureBound;
    typedef Delegate<ShaderProgram*, ShaderStage> StageAdded;

    SamplerAdded samplerAdded;
    SamplerAdded::Signature samplerAddedHandle;
    TextureBound textureBound;
    TextureBound::Signature textureBoundHandle;
    StageAdded stageAdded;
    StageAdded::Signature stageAddedHandle;

    static const int MAX_TEXTURE_SAMPLERS = 16;

    ShaderProgram(VulkanCore* core, vk::Device& device);
    void Compile(vk::Extent2D& swapchainExtent, vk::Format& imageFormat, vk::DescriptorPool& descriptorPool);
    void Cleanup();

    void AddStage(ShaderStage stage);

    vk::Pipeline* GetPipeline();

    void BindDescriptorSet(vk::CommandBuffer& buffer, vk::PipelineBindPoint bindPoint, unsigned int imageIndex);
    void AddSampler();
    void AddSamplers(int amt);
    void SetTexture(vk::ImageView imageView, int index);
    void SetTexture(Texture texture, int index);

protected:
    void BuildRenderPass(vk::Format& imageFormat);
    void BuildDescriptorSetLayout();
    void BuildDescriptorSets(vk::DescriptorPool& descriptorPool, int imageCount, std::vector<vk::Buffer> uniformBuffers);
    void BuildPipeline(vk::Extent2D& extent);

    VulkanCore* core;
    vk::Device* device;

    std::vector<ShaderStage> shaderStages;
    vk::DescriptorSetLayout descriptorSetLayout;
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline pipeline;
    vk::RenderPass renderPass;
    std::vector<vk::DescriptorSet> descriptorSets;

    std::vector<TextureSampler> samplers;
    std::array<vk::ImageView, MAX_TEXTURE_SAMPLERS> textures;
    bool isCompiled;

};

#endif//GLMODELVIEWER_SHADER_H
