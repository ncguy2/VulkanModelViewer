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
#include <plugins/api/Plugin.h>
#include <iostream>
#include <chrono>

extern std::vector<char> ReadFile(const std::string& filename);

void ExampleScreen::Update(float delta) {
    t += delta;

    for (auto &mesh : meshes) {
        mesh->transform = glm::rotate(meshes[0]->transform, delta * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        mesh->data = glm::vec4(fmod((t / 10.0f), 1.0f), 1.0f, 1.0f, 1.0f);
    }
}

void ExampleScreen::Resize(int width, int height) {
    this->width = width;
    this->height = height;

    for (auto &item : shaders)
        item->Recompile(components.core);
}

void ExampleScreen::Show() {
    offscreenColourTextures.resize(MAX_FRAMES_IN_FLIGHT);
    offscreenDepthTextures.resize(MAX_FRAMES_IN_FLIGHT);
    offscreenRenderPass = RenderPass::CreateStandardColourDepthPass(components.core, vk::Extent2D{static_cast<uint32_t>(width), static_cast<uint32_t>(height)}, offscreenColourTextures, offscreenDepthTextures, offscreenDepthTextures.size());

//    std::vector<char> vertShaderCode = ReadFile("assets/shaders/sample/sample.vert");
//    std::vector<char> fragShaderCode = ReadFile("assets/shaders/sample/sample.frag");

    FilePath texPath = L"F:\\Linked\\Downloads\\texture.jpg";
    components.pluginManager->SetDefaultTexture(components.pluginManager->LoadTexture(texPath));


//    mesh->SetVertices({
//                              Vertex({  -0.5f, -0.5f,   0.0f }, { 1.0f, 0.0f, 0.0f }, {1.0f, 0.0f}),
//                              Vertex({   0.5f, -0.5f,   0.0f }, { 0.0f, 1.0f, 0.0f }, {0.0f, 0.0f}),
//                              Vertex({   0.5f,  0.5f,   0.0f }, { 0.0f, 0.0f, 1.0f }, {0.0f, 1.0f}),
//                              Vertex({  -0.5f,  0.5f,   0.0f }, { 1.0f, 1.0f, 1.0f }, {1.0f, 1.0f}),
//
//                              Vertex({  -0.5f, -0.5f,  -0.5f }, { 1.0f, 0.0f, 0.0f }, {1.0f, 0.0f}),
//                              Vertex({   0.5f, -0.5f,  -0.5f }, { 0.0f, 1.0f, 0.0f }, {0.0f, 0.0f}),
//                              Vertex({   0.5f,  0.5f,  -0.5f }, { 0.0f, 0.0f, 1.0f }, {0.0f, 1.0f}),
//                              Vertex({  -0.5f,  0.5f,  -0.5f }, { 1.0f, 1.0f, 1.0f }, {1.0f, 1.0f}),
//                      });
//    mesh->SetIndices({
//                             Triangle(0, 1, 2),
//                             Triangle(2, 3, 0),
//
//                             Triangle(4, 5, 6),
//                             Triangle(6, 7, 4),
//                     });

//    std::filesystem::path path = std::filesystem::u8path(R"(J:/Character Meshes/Genshin Impact/Eula/Eula.pmx)");
//    auto texPath = R"(F:\Linked\Downloads\texture.jpg)";
//    Plugins::ModelLoader *loader = components.core->pluginManager.GetLoader(FileDataType::Model_Type, path);
//    auto data = loader->Load(path)[0];

    RendererSetupContext setupContext{};
    setupContext.core = components.core;
    setupContext.device = components.device;
    setupContext.commandPool = components.commandPool;
    setupContext.textureSet.push_back(offscreenColourTextures[0]->GetView());

    meshRenderer = std::make_shared<MeshRenderer>();
    meshRenderer->Setup(setupContext);

    dropFunc = [this](std::vector<FilePath> paths) {
//        if(paths.size() == 1) {
//            if(paths[0].ends_with(L".png")) {
//                auto selMeshPtr = GetSelectedMesh();
//                if(selMeshPtr) {
//                    std::cout << "Overriding texture in slot " << selectedMeshIdx << " with texture at path: " << CAST_WSTR_STR(paths[0]) << std::endl;
//                    auto texture = components.core->pluginManager.LoadTexture(paths[0]);
//                    selMeshPtr->GetShaderProgram()->SetTexture(texture->GetView(), 0);
//                    textures.push_back(texture);
//                    return;
//                }
//            }
//        }

        auto start = std::chrono::steady_clock::now();
        this->CleanupMeshes();
        auto cleanupTime = std::chrono::steady_clock::now();
        this->LoadMeshes(paths[0]);
        auto end = std::chrono::steady_clock::now();

        std::cout << "Mesh cleanup took " << std::chrono::duration_cast<std::chrono::milliseconds>(cleanupTime - start).count() << "ms" << std::endl;
        std::cout << "Mesh creation took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - cleanupTime).count() << "ms" << std::endl;
    };

    renderFunc = [this](RendererContext& context) {
//        RenderPass* origPass = context.renderPass;



//        context.renderPass = offscreenRenderPass.get();
        context.fbo = context.renderPass->Get(context.bufferIdx).GetVK();
        this->meshRenderer->Render(context);

//        context.renderPass = origPass;
//        context.fbo = origPass->Get(context.bufferIdx).GetVK();
//        blitRenderer->SetTexture(0, offscreenColourTextures[(context.bufferIdx + 1) % offscreenColourTextures.size()].get());
//        this->blitRenderer->Render(context);

        context.commandBuffers.push_back(this->meshRenderer->GetCommandBuffer(context.bufferIdx));
//        context.commandBuffers.push_back(this->blitRenderer->GetCommandBuffer(context.bufferIdx));
    };

    components.core->drop += dropFunc;
    components.core->render += renderFunc;

//    camera = scene.CreateEntity();
//    camera->AddComponent<CameraComponent>();

}
void ExampleScreen::Hide() {
    components.core->render -= renderFunc;
    components.core->drop -= dropFunc;
}

void ExampleScreen::Dispose() {
    CleanupMeshes();
    components.pluginManager->GetDefaultTexture()->Dispose();

    for (auto &item : offscreenColourTextures)
        item->Dispose();

    for (auto &item : offscreenDepthTextures)
        item->Dispose();

    offscreenRenderPass->Dispose();
}

void ExampleScreen::OnKey(int key, int scancode, int action, int mods) {
    if(action != GLFW_PRESS)
        return;

    if(key < GLFW_KEY_A || key > GLFW_KEY_Z)
        return;

    selectedMeshIdx = key - GLFW_KEY_A;
    std::cout << "Selected mesh idx: " << selectedMeshIdx << std::endl;
}

void ExampleScreen::AddMesh(const std::shared_ptr<Mesh>& mesh) {
    meshes.push_back(mesh);
    components.core->staticMeshAdded(components.core, mesh);
}

void ExampleScreen::CleanupMeshes() {

    components.device->waitIdle();

    for (auto &item : meshes)
        item->Dispose();
    for (auto &item : shaders)
        item->Cleanup();
    for (auto &item : textures) {
        if(!components.pluginManager->IsDefaultTexture(item))
            item->Dispose();
    }

//    scene.RemoveEntitiesWith<MeshRendererComponent>();
    scene.RemoveEntities();
    meshes.clear();
    shaders.clear();
    textures.clear();
}

Plugins::FilePath GetParent(Plugins::FilePath path) {
    int idx = path.rfind(L"/");
    if(idx == -1)
        idx = path.rfind(L"\\");
    if(idx == -1)
        return L"./";

    return path.substr(0, idx);
}

void ExampleScreen::LoadMeshes(Plugins::FilePath path) {
    std::vector<char> vertShaderCode = ReadFile("assets/shaders/sample/sample.vert");
    std::vector<char> fragShaderCode = ReadFile("assets/shaders/sample/sample.frag");

    auto data = components.pluginManager->LoadMeshes(path);

    for (const auto &datum : data) {
        auto mesh = std::make_shared<Mesh>();
        mesh->transform = datum.transform;
        mesh->SetVertices(datum.vertices);
        mesh->SetIndices(datum.indices);

        Plugins::FilePath tex = GetParent(path);
        tex += L"/";
        tex += datum.texturePath;

        auto texture = components.core->pluginManager.LoadTexture(tex);
        auto shader = components.core->CreateShaderProgram();
        shader->AddStage(ShaderStage("main", vk::ShaderStageFlagBits::eVertex, vertShaderCode));
        shader->AddStage(ShaderStage("main", vk::ShaderStageFlagBits::eFragment, fragShaderCode));
        shader->AddPushConstant(0, sizeof(MeshVertexPushConstants), vk::ShaderStageFlagBits::eVertex);
        shader->AddSamplers(1);
        shader->SetTexture(texture->GetView(), 0);
        static int shaderCounter = 0;
        shader->SetName("Shader " + std::to_string(shaderCounter++));
        components.core->CompileShader(shader);
        textures.push_back(texture);
        shaders.push_back(shader);

        mesh->SetShaderProgram(shader);
        AddMesh(mesh);

        auto entity = scene.CreateEntity();
        entity->AddComponent<MeshComponent>()->mesh = mesh;
        entity->AddComponent<MeshRendererComponent>();

        entities.push_back(entity);
    }
}
std::shared_ptr<Mesh> ExampleScreen::GetSelectedMesh() {
    if(selectedMeshIdx < 0 || selectedMeshIdx >= meshes.size())
        return nullptr;
    return meshes[selectedMeshIdx];
}
