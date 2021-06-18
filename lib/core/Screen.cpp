//
// Created by Guy on 15/06/2021.
//

#include <core/Screen.h>
#include <data/Shader.h>
#include <data/Mesh.h>

#include <map>

void Screen::Create() {
    isCreated = true;
}
bool Screen::IsCreated() const {
    return isCreated;
}
void Screen::AssignCoreComponents(CoreScreenComponents &c) {
    this->components = c;
}

void ScreenAdapter::Create() {
    Screen::Create();
}

void ScreenAdapter::Update(float delta) {}
void ScreenAdapter::Record(int bufferIdx, vk::CommandBuffer &buffer) {}
void ScreenAdapter::Resize(int width, int height) {}
void ScreenAdapter::Show() {}
void ScreenAdapter::Hide() {}
void ScreenAdapter::Dispose() {}
void ScreenAdapter::OnKey(int key, int scancode, int action, int mods) {}


void ScreenUtils::RecordMeshes(int idx, vk::CommandBuffer &buffer, std::vector<std::shared_ptr<Mesh>>& meshes, CoreScreenComponents& components) {
    std::map<std::shared_ptr<ShaderProgram>, std::vector<std::shared_ptr<Mesh>>> meshMap;
    for (const std::shared_ptr<Mesh>& item : meshes) {
        std::shared_ptr<ShaderProgram> key = item->GetShaderProgram();
        meshMap[key].push_back(item);
    }

    for(const auto& pair : meshMap) {
        std::shared_ptr<ShaderProgram> shader;
        shader = pair.first;

        buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *shader->GetPipeline());
        shader->BindDescriptorSet(buffer, vk::PipelineBindPoint::eGraphics, idx);

        for(const auto& mesh : pair.second) {
            if(!mesh->areBuffersValid) {
                mesh->CreateVertexBuffer(*components.core, *components.device);
                mesh->CreateIndexBuffer(*components.core, *components.device);
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
}
