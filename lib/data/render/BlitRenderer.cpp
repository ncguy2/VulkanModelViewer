//
// Created by Guy on 20/06/2021.
//

#include <data/render/BlitRenderer.h>
#include <data/Mesh.h>

extern std::vector<char> ReadFile(const std::string& filename);

/*
 * vec2 positions[4] = vec2[](
    vec2(-0.5, -0.5),
    vec2( 0.5, -0.5),
    vec2(-0.5,  0.5),
    vec2( 0.5,  0.5)
);

int indexes[6] = int[](
    0, 1, 2,
    1, 3, 2
);
 */

void BlitRenderer::Render(RendererContext &context) {
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
    clearValues[0].color.setFloat32({0, 1, 0, 1});
    clearValues[1].depthStencil.setDepth(1.0f).setStencil(0);
    renderPassInfo.setClearValues(clearValues);

    buffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

    buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *shader->GetPipeline());
    shader->BindDescriptorSet(buffer, vk::PipelineBindPoint::eGraphics, context.bufferIdx);

    vk::Buffer vertexBuffers[] = {mesh->GetVertexBuffer()};
    vk::DeviceSize offsets[] = {0};
    buffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
    buffer.bindIndexBuffer(mesh->GetIndexBuffer(), 0, mesh->GetIndexType());

    buffer.drawIndexed(mesh->GetIndexCount(), 1, 0, 0, 0);

    buffer.endRenderPass();
    buffer.end();
}

void BlitRenderer::Setup(RendererSetupContext &context) {
    AbstractRenderer::Setup(context);

    std::vector<char> vertShaderCode = ReadFile("assets/shaders/blit/blit.vert");
    std::vector<char> fragShaderCode = ReadFile("assets/shaders/blit/blit.frag");

    shader = context.core->CreateShaderProgram();
    shader->AddStage(ShaderStage("main", vk::ShaderStageFlagBits::eVertex, vertShaderCode));
    shader->AddStage(ShaderStage("main", vk::ShaderStageFlagBits::eFragment, fragShaderCode));
    shader->AddSamplers(context.textureSet.size());
    for(int i = 0; i < context.textureSet.size(); i++)
        shader->SetTexture(context.textureSet[i], i);
    context.core->CompileShader(shader);

    mesh = std::make_shared<Mesh>();
    std::vector<Vertex> vertices = {
            Vertex({-1.0f, -1.0f,  0.0f}, {1, 0, 0}, {0, 0}),
            Vertex({ 1.0f, -1.0f,  0.0f}, {0, 1, 0}, {1, 0}),
            Vertex({-1.0f,  1.0f,  0.0f}, {0, 0, 1}, {0, 1}),
            Vertex({ 1.0f,  1.0f,  0.0f}, {1, 1, 1}, {1, 1})
    };
    std::vector<Triangle> indices = {
            Triangle(1, 0, 2),
            Triangle(1, 2, 3),
    };
    mesh->SetVertices(vertices);
    mesh->SetIndices(indices);
    mesh->SetShaderProgram(shader);

    mesh->CreateVertexBuffer(*context.core, *context.device);
    mesh->CreateIndexBuffer(*context.core, *context.device);
}

void BlitRenderer::SetTexture(int slot, Texture *texPtr) {
    shader->SetTexture(texPtr->GetView(), slot);
}
