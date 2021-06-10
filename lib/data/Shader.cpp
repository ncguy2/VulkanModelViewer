//
// Created by Guy on 03/06/2021.
//

#include <data/Shader.h>
#include <glad/glad.h>

#ifndef DEBUG
#define DEBUG 1
#endif

#ifdef DEBUG
#include <data/Mesh.h>
#include <iostream>
#include <utility>
#endif

void ShaderProgram::Cleanup() {
    for (const auto &item : shaderStages)
        item->Cleanup(device);
    shaderStages.clear();
}

ShaderProgram::ShaderProgram(vk::Device *device) : device(device) {}

void ShaderProgram::AddStage(std::shared_ptr<ShaderStage> stage) {
    shaderStages.push_back(stage);
}

void ShaderProgram::Compile(vk::Extent2D swapchainExtent, vk::Format imageFormat) {
    BuildRenderPass(imageFormat);
    BuildPipeline(swapchainExtent);
}

void ShaderProgram::BuildRenderPass(vk::Format imageFormat) {
    vk::AttachmentDescription colourAttachment{};
    colourAttachment.setFormat(imageFormat);
    colourAttachment.setSamples(vk::SampleCountFlagBits::e1);
    colourAttachment.setLoadOp(vk::AttachmentLoadOp::eClear);
    colourAttachment.setStoreOp(vk::AttachmentStoreOp::eStore);
    colourAttachment.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare);
    colourAttachment.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare);
    colourAttachment.setInitialLayout(vk::ImageLayout::eUndefined);
    colourAttachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    vk::AttachmentReference colourAttachmentRef{};
    colourAttachmentRef.setAttachment(0);
    colourAttachmentRef.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription subpass{};
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
    subpass.setColorAttachmentCount(1);
    subpass.setPColorAttachments(&colourAttachmentRef);

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
    renderPassInfo.setAttachmentCount(1);
    renderPassInfo.setPAttachments(&colourAttachment);
    renderPassInfo.setSubpassCount(1);
    renderPassInfo.setPSubpasses(&subpass);
    renderPassInfo.setDependencyCount(2);
    renderPassInfo.setPDependencies(dependencies);

    if(device->createRenderPass(&renderPassInfo, nullptr, &renderPass) != vk::Result::eSuccess)
        throw std::runtime_error("Failed to create render pass");
}

void ShaderProgram::BuildPipeline(vk::Extent2D extent) {
    vk::PipelineShaderStageCreateInfo shaderStageInfos[shaderStages.size()];
    int i = 0;
    for (const auto &item : shaderStages)
        shaderStageInfos[i++] = item->Create(device);

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
    rasterizer.setFrontFace(vk::FrontFace::eClockwise);
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
            vk::DynamicState::eLineWidth
    };

    vk::PipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.setDynamicStateCount(2);
    dynamicState.setPDynamicStates(dynamicStates);

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};

    if(device->createPipelineLayout(&pipelineLayoutInfo, nullptr, &pipelineLayout) != vk::Result::eSuccess)
        throw std::runtime_error("Failed to create pipeline layout");

    vk::GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.setStageCount(2);
    pipelineInfo.setPStages(shaderStageInfos);
    pipelineInfo.setPVertexInputState(&vertexInputInfo);
    pipelineInfo.setPInputAssemblyState(&inputAssembly);
    pipelineInfo.setPViewportState(&viewportState);
    pipelineInfo.setPRasterizationState(&rasterizer);
    pipelineInfo.setPMultisampleState(&multisampling);
    pipelineInfo.setPDepthStencilState(nullptr);
    pipelineInfo.setPColorBlendState(&colourBlending);
//    pipelineInfo.setPDynamicState(&dynamicState);
    pipelineInfo.setLayout(pipelineLayout);
    pipelineInfo.setRenderPass(renderPass);
    pipelineInfo.setSubpass(0);

    if(device->createGraphicsPipelines(nullptr, 1, &pipelineInfo, nullptr, &pipeline) != vk::Result::eSuccess)
        throw std::runtime_error("Failed to create graphics pipeline");
}
