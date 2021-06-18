//
// Created by Guy on 03/06/2021.
//
#include <pch.h>

#include <data/Shader.h>
#include <glad/glad.h>

#ifndef DEBUG
#define DEBUG 1
#endif

#ifdef DEBUG
#include <core/VulkanCore.hpp>
#include <data/Mesh.h>
#include <iostream>
#include <utility>
#endif

void ShaderProgram::Cleanup() {

    for (auto &item : samplers)
        item.Dispose();

    device->destroyDescriptorSetLayout(descriptorSetLayout);

    for (auto &item : shaderStages)
        item.Cleanup(device);
    shaderStages.clear();
}

ShaderProgram::ShaderProgram(VulkanCore *core, vk::Device &device) : core(core), device(&device), isCompiled(false) {

    samplerAdded += samplerAddedHandle = [](ShaderProgram *program, TextureSampler sampler) {
        if (program->isCompiled)
            program->core->CompileShader(program);
    };
    textureBound += textureBoundHandle = [](ShaderProgram *program, vk::ImageView view, int slot) {
        if (program->isCompiled)
            program->core->CompileShader(program);
    };
    stageAdded += stageAddedHandle = [](ShaderProgram *program, ShaderStage stage) {
        if (program->isCompiled)
            program->core->CompileShader(program);
    };
}

void ShaderProgram::AddStage(ShaderStage stage) {
    shaderStages.push_back(stage);
    stageAdded(this, stage);
}

void ShaderProgram::Compile(vk::Extent2D &swapchainExtent, vk::Format &imageFormat, vk::DescriptorPool &descriptorPool, vk::RenderPass& renderPass) {
    BuildDescriptorSetLayout();
    BuildDescriptorSets(descriptorPool, core->ImageCount(), core->GetUniformBuffers());
    BuildPipeline(swapchainExtent, renderPass);
    isCompiled = true;
}

void ShaderProgram::BuildDescriptorSetLayout() {
    vk::DescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.setBinding(0);
    uboLayoutBinding.setDescriptorType(vk::DescriptorType::eUniformBuffer);
    uboLayoutBinding.setDescriptorCount(1);
    uboLayoutBinding.setStageFlags(vk::ShaderStageFlagBits::eVertex);
    uboLayoutBinding.setPImmutableSamplers(nullptr);

    vk::DescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.setBinding(1);
    samplerLayoutBinding.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
    samplerLayoutBinding.setDescriptorCount(samplers.size());
    samplerLayoutBinding.setPImmutableSamplers(nullptr);
    samplerLayoutBinding.setStageFlags(vk::ShaderStageFlagBits::eFragment);

    std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

    vk::DescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.setBindings(bindings);

    CHECK(device->createDescriptorSetLayout(&layoutInfo, nullptr, &descriptorSetLayout));
}

void ShaderProgram::BuildDescriptorSets(vk::DescriptorPool &descriptorPool, int imageCount, std::vector<vk::Buffer> uniformBuffers) {
    std::vector<vk::DescriptorSetLayout> layouts(imageCount, descriptorSetLayout);
    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.setDescriptorPool(descriptorPool);
    allocInfo.setSetLayouts(layouts);

    descriptorSets.resize(imageCount);
    CHECK(device->allocateDescriptorSets(&allocInfo, descriptorSets.data()));

    for (size_t i = 0; i < imageCount; i++) {
        vk::DescriptorBufferInfo bufferInfo{};
        bufferInfo.setBuffer(uniformBuffers[i]);
        bufferInfo.setOffset(0);
        bufferInfo.setRange(sizeof(UniformBufferObject));

        if (!samplers.empty()) {

            std::vector<vk::DescriptorImageInfo> imageInfos(samplers.size());
            int idx = 0;
            for (const TextureSampler &item : samplers) {
                imageInfos[idx] = item.Prepare(textures[idx]);
                idx++;
            }

            std::array<vk::WriteDescriptorSet, 2> descriptorWrites;

            descriptorWrites[0].setDstSet(descriptorSets[i]);
            descriptorWrites[0].setDstBinding(0);
            descriptorWrites[0].setDstArrayElement(0);
            descriptorWrites[0].setDescriptorType(vk::DescriptorType::eUniformBuffer);
            descriptorWrites[0].setDescriptorCount(1);
            descriptorWrites[0].setPBufferInfo(&bufferInfo);

            descriptorWrites[1].setDstSet(descriptorSets[i]);
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
            descriptorWrites[1].setImageInfo(imageInfos);

            device->updateDescriptorSets(descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
        } else {
            vk::WriteDescriptorSet descriptorWrite{};
            descriptorWrite.setDstSet(descriptorSets[i]);
            descriptorWrite.setDstBinding(0);
            descriptorWrite.setDstArrayElement(0);
            descriptorWrite.setDescriptorType(vk::DescriptorType::eUniformBuffer);
            descriptorWrite.setDescriptorCount(1);
            descriptorWrite.setPBufferInfo(&bufferInfo);

            device->updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
        }
    }
}

void ShaderProgram::BuildPipeline(vk::Extent2D &extent, vk::RenderPass& renderPass) {
    std::vector<vk::PipelineShaderStageCreateInfo> infoVector;
    vk::PipelineShaderStageCreateInfo *shaderStageInfos;
    for (auto &item : shaderStages)
        infoVector.push_back(item.Create(device));
    //    std::copy(infoVector.begin(), infoVector.end(), shaderStageInfos);
    shaderStageInfos = &infoVector[0];

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescription = Vertex::getAttributeDescriptions();

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.setVertexBindingDescriptionCount(1);
    vertexInputInfo.setPVertexBindingDescriptions(&bindingDescription);
    vertexInputInfo.setVertexAttributeDescriptions(attributeDescription);

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);
    inputAssembly.setPrimitiveRestartEnable(VK_FALSE);

    vk::Viewport viewport{};
    viewport.setX(0).setY(0);
    viewport.setWidth((float) extent.width).setHeight((float) extent.height);
    viewport.setMinDepth(0).setMaxDepth(1);

    vk::Rect2D scissor{};
    scissor.setOffset({0, 0});
    scissor.setExtent(extent);

    vk::PipelineViewportStateCreateInfo viewportState{};
    viewportState.setViewportCount(1);
    viewportState.setPViewports(&viewport);
    viewportState.setScissorCount(1);
    viewportState.setPScissors(&scissor);

    vk::PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.setDepthClampEnable(VK_FALSE);
    rasterizer.setRasterizerDiscardEnable(VK_FALSE);
    rasterizer.setPolygonMode(vk::PolygonMode::eFill);
    rasterizer.setLineWidth(1);
    rasterizer.setCullMode(vk::CullModeFlagBits::eBack);
    rasterizer.setFrontFace(vk::FrontFace::eCounterClockwise);
    rasterizer.setDepthBiasEnable(VK_FALSE);

    vk::PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.setSampleShadingEnable(VK_FALSE);
    multisampling.setRasterizationSamples(vk::SampleCountFlagBits::e1);

    // Per-framebuffer blending settings
    vk::PipelineColorBlendAttachmentState colourBlendAttachment{};
    colourBlendAttachment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
    colourBlendAttachment.setBlendEnable(VK_TRUE);
    colourBlendAttachment.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
    colourBlendAttachment.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
    colourBlendAttachment.setColorBlendOp(vk::BlendOp::eAdd);
    colourBlendAttachment.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
    colourBlendAttachment.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
    colourBlendAttachment.setAlphaBlendOp(vk::BlendOp::eAdd);

    // Global blending settings
    vk::PipelineColorBlendStateCreateInfo colourBlending{};
    colourBlending.setLogicOpEnable(VK_FALSE);
    colourBlending.setAttachmentCount(1);
    colourBlending.setPAttachments(&colourBlendAttachment);

    vk::DynamicState dynamicStates[] = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eLineWidth};

    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.setDynamicStateCount(2);
    dynamicState.setPDynamicStates(dynamicStates);

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.setSetLayoutCount(1);
    pipelineLayoutInfo.setPSetLayouts(&descriptorSetLayout);

    vk::PushConstantRange pushConstantInfo{};
    pushConstantInfo.offset = 0;
    pushConstantInfo.size = sizeof(MeshVertexPushConstants);
    pushConstantInfo.stageFlags = vk::ShaderStageFlagBits::eVertex;

    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantInfo;

    if (device->createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipelineLayout) != vk::Result::eSuccess)
        throw std::runtime_error("Failed to create pipeline layout");

    vk::PipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = vk::CompareOp::eLess;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f;
    depthStencil.maxDepthBounds = 1.0f;
    depthStencil.stencilTestEnable = VK_FALSE;

    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.setStageCount(infoVector.size());
    pipelineInfo.setPStages(shaderStageInfos);
    pipelineInfo.setPVertexInputState(&vertexInputInfo);
    pipelineInfo.setPInputAssemblyState(&inputAssembly);
    pipelineInfo.setPViewportState(&viewportState);
    pipelineInfo.setPRasterizationState(&rasterizer);
    pipelineInfo.setPMultisampleState(&multisampling);
    pipelineInfo.setPDepthStencilState(&depthStencil);
    pipelineInfo.setPColorBlendState(&colourBlending);
    //    pipelineInfo.setPDynamicState(&dynamicState);
    pipelineInfo.setLayout(pipelineLayout);
    pipelineInfo.setRenderPass(renderPass);
    pipelineInfo.setSubpass(0);

    if (device->createGraphicsPipelines(nullptr, 1, &pipelineInfo, nullptr, &pipeline) != vk::Result::eSuccess)
        throw std::runtime_error("Failed to create graphics pipeline");
}

vk::Pipeline *ShaderProgram::GetPipeline() {
    return &pipeline;
}

void ShaderProgram::BindDescriptorSet(vk::CommandBuffer &buffer, vk::PipelineBindPoint bindPoint, unsigned int imageIndex) {
    buffer.bindDescriptorSets(bindPoint, pipelineLayout, 0, 1, &descriptorSets[imageIndex], 0, nullptr);
}

void ShaderProgram::AddSampler() {
    samplers.emplace_back(core, device).Create();
    samplerAdded(this, samplers.back());
}

void ShaderProgram::AddSamplers(int amt) {
    for (int i = 0; i < amt; i++)
        AddSampler();
}

void ShaderProgram::SetTexture(Texture texture, int index) {
    SetTexture(texture.GetView(), index);
}
void ShaderProgram::SetTexture(vk::ImageView imageView, int index) {
    textures[index] = imageView;
    textureBound(this, imageView, index);
}
vk::PipelineLayout &ShaderProgram::GetLayout() {
    return pipelineLayout;
}
void ShaderProgram::Recompile(VulkanCore* core) {
    device->destroyDescriptorSetLayout(descriptorSetLayout);

    core->CompileShader(this);
}
