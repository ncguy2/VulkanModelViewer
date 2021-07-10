//
// Created by Guy on 07/07/2021.
//

#include <data/render/GridRenderer.h>
#include <data/Contexts.h>
#include <data/Mesh.h>

extern std::vector<char> ReadFile(const std::string& filename);


void GridRenderer::Render(RendererContext &context) {
    context.buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *shader->GetPipeline());
    shader->BindDescriptorSet(context.buffer, vk::PipelineBindPoint::eGraphics, context.bufferIdx);

    context.buffer.draw(6, 1, 0, 0);
}

void GridRenderer::Setup(RendererSetupContext &context) {
    AbstractRenderer::Setup(context);

    std::vector<char> vertShaderCode = ReadFile("assets/shaders/grid/grid.vert");
    std::vector<char> fragShaderCode = ReadFile("assets/shaders/grid/grid.frag");

    shader = context.core->CreateShaderProgram();
    shader->AddStage(ShaderStage("main", vk::ShaderStageFlagBits::eVertex, vertShaderCode));
    shader->AddStage(ShaderStage("main", vk::ShaderStageFlagBits::eFragment, fragShaderCode));

    shader->SetName("Grid Shader");
    context.core->CompileShader(shader);

//    mesh = std::make_shared<Mesh>();
//    mesh->SetVertices({
//            Vertex({1, 1, 0}, {}, {}),
//            Vertex({-1, -1, 0}, {}, {}),
//            Vertex({-1, 1, 0}, {}, {}),
//            Vertex({-1, -1, 0}, {}, {}),
//            Vertex({1, 1, 0}, {}, {}),
//            Vertex({1, -1, 0}, {}, {}),
//    });
//    mesh->SetIndices({{0, 1, 2}, {3, 4, 5}});
//
//    mesh->SetShaderProgram(shader);
//    mesh->CreateVertexBuffer(*context.core, *context.device);
//    mesh->CreateIndexBuffer(*context.core, *context.device);
}

void GridRenderer::Dispose() {
    AbstractRenderer::Dispose();
    shader->Cleanup();
}

const char *GridRenderer::Name() {
    return "Grid Renderer";
}
void GridRenderer::Resize(int width, int height) {
    AbstractRenderer::Resize(width, height);
    shader->Recompile(setupContext.core);
}
