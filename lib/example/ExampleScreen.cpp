//
// Created by Guy on 15/06/2021.
//

#include <example/ExampleScreen.h>
#include <core/VulkanCore.hpp>

#include <GLFW/glfw3.h>
#include <data/Mesh.h>
#include <data/Shader.h>
#include <data/render/BlitRenderer.h>
#include <data/render/MeshRenderer.h>
#include <ecs/Entity.h>
#include <ecs/components/CameraComponent.h>
#include <ecs/components/MeshComponent.h>
#include <ecs/components/MeshRendererComponent.h>
#include <map>
#include <memory>

extern std::vector<char> ReadFile(const std::string& filename);

void ExampleScreen::Update(float delta) {
    t += delta;
    meshes[0]->transform = glm::rotate(meshes[0]->transform, delta * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    meshes[0]->data = glm::vec4(fmod((t / 10.0f), 1.0f), 1.0f, 1.0f, 1.0f);
}

void ExampleScreen::Resize(int width, int height) {
    this->width = width;
    this->height = height;

    if(shader)
        shader->Recompile(components.core);
}

void ExampleScreen::Show() {
    offscreenColourTextures.resize(MAX_FRAMES_IN_FLIGHT);
    offscreenDepthTextures.resize(MAX_FRAMES_IN_FLIGHT);
    offscreenRenderPass = RenderPass::CreateStandardColourDepthPass(components.core, vk::Extent2D{static_cast<uint32_t>(width), static_cast<uint32_t>(height)}, offscreenColourTextures, offscreenDepthTextures, offscreenDepthTextures.size());

    std::vector<char> vertShaderCode = ReadFile("assets/shaders/sample/sample.vert");
    std::vector<char> fragShaderCode = ReadFile("assets/shaders/sample/sample.frag");

    shader = components.core->CreateShaderProgram();
    shader->AddStage(ShaderStage("main", vk::ShaderStageFlagBits::eVertex, vertShaderCode));
    shader->AddStage(ShaderStage("main", vk::ShaderStageFlagBits::eFragment, fragShaderCode));
    shader->AddSamplers(1);

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

    mesh->SetVertices({
                              Vertex({  -0.5f, -0.5f,   0.0f }, { 1.0f, 0.0f, 0.0f }, {1.0f, 0.0f}),
                              Vertex({   0.5f, -0.5f,   0.0f }, { 0.0f, 1.0f, 0.0f }, {0.0f, 0.0f}),
                              Vertex({   0.5f,  0.5f,   0.0f }, { 0.0f, 0.0f, 1.0f }, {0.0f, 1.0f}),
                              Vertex({  -0.5f,  0.5f,   0.0f }, { 1.0f, 1.0f, 1.0f }, {1.0f, 1.0f}),

                              Vertex({  -0.5f, -0.5f,  -0.5f }, { 1.0f, 0.0f, 0.0f }, {1.0f, 0.0f}),
                              Vertex({   0.5f, -0.5f,  -0.5f }, { 0.0f, 1.0f, 0.0f }, {0.0f, 0.0f}),
                              Vertex({   0.5f,  0.5f,  -0.5f }, { 0.0f, 0.0f, 1.0f }, {0.0f, 1.0f}),
                              Vertex({  -0.5f,  0.5f,  -0.5f }, { 1.0f, 1.0f, 1.0f }, {1.0f, 1.0f}),
                      });
    mesh->SetIndices({
                             Triangle(0, 1, 2),
                             Triangle(2, 3, 0),

                             Triangle(4, 5, 6),
                             Triangle(6, 7, 4),
                     });

    mesh->SetShaderProgram(shader);
    texture = components.core->CreateTexture(vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
    texture->LoadFromFile(R"(F:\Linked\Downloads\texture.jpg)");
    shader->SetTexture(texture->GetView(), 0);
    components.core->CompileShader(shader);

    AddMesh(mesh);

    entity = scene.CreateEntity();

    entity->AddComponent<MeshComponent>()->mesh = meshes[0];
    entity->AddComponent<MeshRendererComponent>();


    RendererSetupContext setupContext{};
    setupContext.core = components.core;
    setupContext.device = components.device;
    setupContext.commandPool = components.commandPool;
    setupContext.textureSet.push_back(offscreenColourTextures[0]->GetView());

    meshRenderer = std::make_shared<MeshRenderer>();
    meshRenderer->Setup(setupContext);

    blitRenderer = std::make_shared<BlitRenderer>();
    blitRenderer->Setup(setupContext);
    blitRenderer->SetTexture(0, offscreenColourTextures[0].get());

    renderFunc = [this](RendererContext& context) {
        RenderPass* origPass = context.renderPass;

        context.renderPass = offscreenRenderPass.get();
        context.fbo = context.renderPass->Get(context.bufferIdx).GetVK();
        this->meshRenderer->Render(context);

        context.renderPass = origPass;
        context.fbo = origPass->Get(context.bufferIdx).GetVK();
        blitRenderer->SetTexture(0, offscreenColourTextures[context.bufferIdx].get());
        this->blitRenderer->Render(context);

        context.commandBuffers.push_back(this->meshRenderer->GetCommandBuffer(context.bufferIdx));
        context.commandBuffers.push_back(this->blitRenderer->GetCommandBuffer(context.bufferIdx));
    };

    components.core->render += renderFunc;

//    camera = scene.CreateEntity();
//    camera->AddComponent<CameraComponent>();

}
void ExampleScreen::Hide() {
    components.core->render -= renderFunc;
}

void ExampleScreen::Dispose() {
    for (auto &item : meshes)
        item->Dispose();

    offscreenRenderPass->Dispose();
    shader->Cleanup();
    texture->Dispose();
}

void ExampleScreen::OnKey(int key, int scancode, int action, int mods) {
}

void ExampleScreen::AddMesh(const std::shared_ptr<Mesh>& mesh) {
    meshes.push_back(mesh);
    components.core->staticMeshAdded(components.core, mesh);
}
