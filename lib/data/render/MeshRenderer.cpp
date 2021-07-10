//
// Created by Guy on 20/06/2021.
//


#include <core/Screen.h>
#include <data/render/MeshRenderer.h>
#include <data/Mesh.h>
#include <map>

void MeshRenderer::Render(RendererContext &context) {
    std::map<std::shared_ptr<ShaderProgram>, std::vector<std::shared_ptr<Mesh>>> meshMap;
    for (const std::shared_ptr<Mesh>& item : context.data.meshesToRender) {
        std::shared_ptr<ShaderProgram> key = item->GetShaderProgram();
        meshMap[key].push_back(item);
    }

    for(const auto& pair : meshMap) {
        std::shared_ptr<ShaderProgram> shader;
        shader = pair.first;

        context.buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *shader->GetPipeline());
        shader->BindDescriptorSet(context.buffer, vk::PipelineBindPoint::eGraphics, context.bufferIdx);

        for(const auto& mesh : pair.second) {
            if(!mesh->areBuffersValid) {
                mesh->CreateVertexBuffer(*context.core, *context.device);
                mesh->CreateIndexBuffer(*context.core, *context.device);
                mesh->areBuffersValid = true;
            }
            vk::Buffer vertexBuffers[] = {mesh->GetVertexBuffer()};
            vk::DeviceSize offsets[] = {0};
            context.buffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
            context.buffer.bindIndexBuffer(mesh->GetIndexBuffer(), 0, mesh->GetIndexType());

            MeshVertexPushConstants constants{};
            constants.model = mesh->transform;
            constants.data = mesh->data;
            context.buffer.pushConstants(shader->GetLayout(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(MeshVertexPushConstants), &constants);

            context.buffer.drawIndexed(mesh->GetIndexCount(), 1, 0, 0, 0);
        }
    }
}

const char *MeshRenderer::Name() {
    return "Mesh Renderer";
}
