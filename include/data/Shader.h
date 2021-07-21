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
#include "VertexDescription.h"
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

struct MeshVertexPushConstants {
    glm::mat4 model;
    glm::vec4 data;
};

struct UniformBufferObject {
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec4 cameraData;
};

struct LightParameters {
    glm::vec4 lightPos;
    glm::vec4 viewPos;
    glm::vec4 colour;
    float ambientStrength;
    float specularStrength;
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
    void Compile(vk::Extent2D& swapchainExtent, vk::Format& imageFormat, vk::DescriptorPool& descriptorPool, vk::RenderPass& renderPass);
    void Cleanup();

    void AddStage(ShaderStage stage);

    vk::Pipeline* GetPipeline();

    void BindDescriptorSet(vk::CommandBuffer& buffer, vk::PipelineBindPoint bindPoint, unsigned int imageIndex);
    void Recompile(VulkanCore* core);
    void AddSampler();
    void AddSamplers(int amt);
    void SetTexture(vk::ImageView imageView, int index);
    void SetTexture(Texture texture, int index);

    void SetViewport(vk::Viewport viewport);
    void SetScissor(vk::Rect2D scissor);
    void ResetScissor();

    vk::PipelineLayout& GetLayout();

    void SetName(std::string name);

    void AddPushConstant(int offset, int size, vk::ShaderStageFlags stageFlags);

    void AddVertexDescription(VertexDescription desc);

protected:
    void BuildDescriptorSetLayout();
    void BuildDescriptorSets(vk::DescriptorPool& descriptorPool, int imageCount, std::vector<vk::Buffer> uniformBuffers);
    void BuildPipeline(vk::Extent2D& extent, vk::RenderPass& renderPass);

    VulkanCore* core;
    vk::Device* device;

    std::string name;
    std::vector<ShaderStage> shaderStages;
    vk::DescriptorSetLayout descriptorSetLayout;
    vk::PipelineLayout pipelineLayout;
    vk::Pipeline pipeline;
    vk::Viewport viewport;
    vk::Rect2D scissor;
    std::vector<vk::DescriptorSet> descriptorSets;

    std::vector<TextureSampler> samplers;
    std::array<vk::ImageView, MAX_TEXTURE_SAMPLERS> textures;
    bool isCompiled;

    void UpdateDescriptorSets();
    int imageCount;
    std::vector<vk::Buffer> uniformBuffers;
    std::vector<vk::PushConstantRange> pushConstantInfo;

    std::vector<VertexDescription> vertDescriptions;
};

#endif//GLMODELVIEWER_SHADER_H
