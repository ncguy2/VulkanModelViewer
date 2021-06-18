//
// Created by Guy on 15/06/2021.
//

#include <example/ExampleScreen.h>
#include <core/VulkanCore.hpp>

#include <GLFW/glfw3.h>
#include <data/Mesh.h>
#include <data/Shader.h>
#include <map>

extern std::vector<char> ReadFile(const std::string& filename);

void ExampleScreen::Update(float delta) {
    scene.Update(delta);

    t += delta;
    meshes[0]->transform = glm::rotate(meshes[0]->transform, delta * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    meshes[0]->data = glm::vec4(fmod((t / 10.0f), 1.0f), 1.0f, 1.0f, 1.0f);
}
void ExampleScreen::Record(int i, vk::CommandBuffer &buffer) {
    scene.Record(i, buffer, components.core);
    ScreenUtils::RecordMeshes(i, buffer, meshes, components);
}

void ExampleScreen::Resize(int width, int height) {
    shader->Recompile(components.core);
}

void ExampleScreen::Show() {
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
}
void ExampleScreen::Hide() {
}
void ExampleScreen::Dispose() {
    for (auto &item : meshes)
        item->Dispose();

    shader->Cleanup();
    texture.reset();
}

void ExampleScreen::OnKey(int key, int scancode, int action, int mods) {
}

void ExampleScreen::AddMesh(const std::shared_ptr<Mesh>& mesh) {
    meshes.push_back(mesh);
    components.core->staticMeshAdded(components.core, mesh);
}
