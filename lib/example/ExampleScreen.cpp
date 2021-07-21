//
// Created by Guy on 15/06/2021.
//

#include <Logging.h>

#include <example/ExampleScreen.h>
#include <core/VulkanCore.hpp>

#include <GLFW/glfw3.h>
#include <data/Mesh.h>
#include <data/Shader.h>
#include <data/render/MeshRenderer.h>
#include <data/render/GridRenderer.h>
#include <ecs/Entity.h>
#include <data/vk/Camera.h>
#include <ecs/components/CameraComponent.h>
#include <ecs/components/FlyCamComponent.h>
#include <ecs/components/NameComponent.h>
#include <ecs/components/MeshComponent.h>
#include <ecs/components/MeshRendererComponent.h>
#include <map>
#include <memory>
#include <plugins/api/Plugin.h>
#include <iostream>
#include <chrono>
#include <interop/InteropHost.h>
#include <interop/MetadataRecord.h>

extern std::vector<char> ReadFile(const std::string& filename);
extern InteropHost interop;

void ExampleScreen::Update(UpdateContext& context) {
    t += context.delta;

    if(context.GetKey(GLFW_KEY_KP_ADD))
        gridScale += context.delta * 10;
    if(context.GetKey(GLFW_KEY_KP_SUBTRACT))
        gridScale -= context.delta * 10;

    if(gridScale < 1)
        gridScale = 1;

    if(context.GetKey(GLFW_KEY_R)) {
        gridScale = (gridScale + (10.0f - gridScale) * (context.delta * 10));
    }
//        gridScale = glm::lerp(gridScale, 10.0f, context.delta * 10);

    if(cameraPtr) {
        context.cameraData.x = cameraPtr->GetNear();
        context.cameraData.y = cameraPtr->GetFar();
        context.cameraData.z = gridScale;
    }

//    for (auto &mesh : meshes) {
//        mesh->transform = glm::rotate(meshes[0]->transform, delta * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//        mesh->data = glm::vec4(fmod((t / 10.0f), 1.0f), 1.0f, 1.0f, 1.0f);
//    }

}

void ExampleScreen::Resize(int width, int height) {
    this->width = width;
    this->height = height;

    for (auto &item : shaders)
        item->Recompile(components.core);

    rendererStack.Resize(width, height);

    PerspectiveSettings ps{};
    ps.width = width;
    ps.height = height;
    ps.fov = 90;
    ps.nearPlane = 0.01f;
    ps.farPlane = 100;

    if(cameraPtr)
        cameraPtr->SetPerspective(ps);
}

void ExampleScreen::Show() {
    FilePath texPath = L"default-texture.png";
    components.pluginManager->SetDefaultTexture(components.pluginManager->LoadTexture(texPath));

    RendererSetupContext setupContext{};
    setupContext.core = components.core;
    setupContext.device = components.device;
    setupContext.commandPool = components.commandPool;

    rendererStack.AddRenderer<MeshRenderer>();
    rendererStack.AddRenderer<GridRenderer>();
    rendererStack.Setup(setupContext);

    dropFunc = [this](std::vector<FilePath> paths) {
        auto start = std::chrono::steady_clock::now();
        this->CleanupMeshes();
        auto cleanupTime = std::chrono::steady_clock::now();
      std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;

      Logging::Get() << "Loading mesh from " << convert.to_bytes(paths[0]) << std::endl;

        auto data = components.pluginManager->LoadMeshes(paths[0]);
        LOG_LN("Meshes loaded from file: " + std::to_string(data.size()));
        int idx = 0;
        for (auto &datum : data) {
            LOG_LN("Loading mesh from data[" + std::to_string(idx++) + "]");
            LoadMesh(datum);
        }
      LOG_LN("LoadMesh Success");

        auto end = std::chrono::steady_clock::now();

      Logging::Get() << "Mesh cleanup took " << std::chrono::duration_cast<std::chrono::milliseconds>(cleanupTime - start).count() << "ms" << std::endl;
      Logging::Get() << "Mesh creation took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - cleanupTime).count() << "ms" << std::endl;

      std::vector<std::string> lines(4);
      lines[0] = std::string("Mesh path: " + convert.to_bytes(meshPath));
      lines[1] = std::string("Mesh count: " + std::to_string(meshes.size()));
      lines[2] = std::string("Texture count: " + std::to_string(textures.size()));
      lines[3] = std::string("Mesh creation took " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(end - cleanupTime).count()) + "ms");

      components.core->SetModelInfo(lines);

      metadata.ClearRecords();

      Record<Records::MeshData>* r = new Record<Records::MeshData>();
      r->type = RecordType::ModelData;
      r->size = sizeof(Records::MeshData);
      r->data.meshCount = meshes.size();
      r->data.textureCount = textures.size();
      r->data.meshCreationTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - cleanupTime).count();
//      r.data.meshPath = convert.to_bytes(meshPath).c_str();

      interop.Call_MetadataSource(r);

      delete r;
    };

    renderFunc = [this](RendererContext& context) {
        context.fbo = context.renderPass->Get(context.bufferIdx).GetVK();
        rendererStack.Render(context);
    };

    cameraEntity = scene.CreateEntity();
    cameraEntity->AddComponent<NameComponent>()->name = "Camera";
    cameraEntity->AddComponent<FlyCamComponent>();
    cameraPtr = cameraEntity->AddComponent<CameraComponent>()->GetCameraPtr();
    cameraEntity->transform.translation = glm::vec3(5, 5, 5);

    components.core->drop += dropFunc;
    components.core->render += renderFunc;
}
void ExampleScreen::Hide() {
    components.core->render -= renderFunc;
    components.core->drop -= dropFunc;
}

void ExampleScreen::Dispose() {
    rendererStack.Dispose();
    CleanupMeshes();
    auto texPtr = components.pluginManager->GetDefaultTexture();
    if(texPtr) {
        texPtr->Dispose();
        texPtr = nullptr;
    }
}

void ExampleScreen::OnKey(int key, int scancode, int action, int mods) {
    if(action != GLFW_PRESS)
        return;

    if(key >= GLFW_KEY_1 && key <= GLFW_KEY_9) {
        rendererStack.Toggle(key - GLFW_KEY_1);
    }
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

    scene.RemoveEntitiesWith<MeshRendererComponent>();
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

std::shared_ptr<Mesh> ExampleScreen::GetSelectedMesh() {
    if(selectedMeshIdx < 0 || selectedMeshIdx >= meshes.size())
        return nullptr;
    return meshes[selectedMeshIdx];
}
void ExampleScreen::LoadMesh(MeshData& datum) {

    meshPath = datum.meshPath;

    // TODO make dependant on material parameters
    std::vector<char> vertShaderCode = ReadFile("assets/shaders/sample/sample.vert");
    std::vector<char> fragShaderCode = ReadFile("assets/shaders/sample/sample.frag");

    auto mesh = std::make_shared<Mesh>();
    mesh->transform = datum.transform;
    mesh->SetVertices(datum.vertices);
    mesh->SetIndices(datum.indices);

    Plugins::FilePath tex = GetParent(datum.meshPath);
    tex += L"/";
    tex += datum.texturePath;

    auto texture = components.core->pluginManager.LoadTexture(tex);
    auto shader = components.core->CreateShaderProgram();
    shader->AddStage(ShaderStage("main", vk::ShaderStageFlagBits::eVertex, vertShaderCode));
    shader->AddStage(ShaderStage("main", vk::ShaderStageFlagBits::eFragment, fragShaderCode));

    VertexDescription vertDesc;
    vertDesc.setInputRate(vk::VertexInputRate::eVertex);
    int stride = sizeof(Vertex);
    vertDesc.setStride(stride);
    vertDesc.Add(vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos));
    vertDesc.Add(vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal));
    vertDesc.Add(vk::Format::eR32G32Sfloat, offsetof(Vertex, uv));

    shader->AddVertexDescription(vertDesc);

    shader->AddPushConstant(0, sizeof(MeshVertexPushConstants), vk::ShaderStageFlagBits::eVertex);
    shader->AddPushConstant(sizeof(MeshVertexPushConstants), sizeof(LightParameters), vk::ShaderStageFlagBits::eFragment);
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

