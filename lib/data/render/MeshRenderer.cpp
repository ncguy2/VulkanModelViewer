//
// Created by Guy on 20/06/2021.
//


#include <pch.h>
#include <core/Screen.h>
#include <data/render/MeshRenderer.h>
#include <data/Shader.h>
#include <data/Mesh.h>
#include <data/RenderPass.h>
#include <map>

void MeshRenderer::Render(RendererContext &context) {
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

    std::map<std::shared_ptr<ShaderProgram>, std::vector<std::shared_ptr<Mesh>>> meshMap;
    for (const std::shared_ptr<Mesh>& item : context.data.meshesToRender) {
        std::shared_ptr<ShaderProgram> key = item->GetShaderProgram();
        meshMap[key].push_back(item);
    }

    buffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

    for(const auto& pair : meshMap) {
        std::shared_ptr<ShaderProgram> shader;
        shader = pair.first;

        buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *shader->GetPipeline());
        shader->BindDescriptorSet(buffer, vk::PipelineBindPoint::eGraphics, context.bufferIdx);

        for(const auto& mesh : pair.second) {
            if(!mesh->areBuffersValid) {
                mesh->CreateVertexBuffer(*context.core, *context.device);
                mesh->CreateIndexBuffer(*context.core, *context.device);
                mesh->areBuffersValid = true;
            }
            vk::Buffer vertexBuffers[] = {mesh->GetVertexBuffer()};
            vk::DeviceSize offsets[] = {0};
            buffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
            buffer.bindIndexBuffer(mesh->GetIndexBuffer(), 0, mesh->GetIndexType());

            MeshVertexPushConstants constants{};
            constants.model = mesh->transform;
            constants.data = mesh->data;
            buffer.pushConstants(shader->GetLayout(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(MeshVertexPushConstants), &constants);

            buffer.drawIndexed(mesh->GetIndexCount(), 1, 0, 0, 0);
        }
    }

    buffer.endRenderPass();
    buffer.end();
}
