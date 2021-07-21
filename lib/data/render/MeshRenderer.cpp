//
// Created by Guy on 20/06/2021.
//


#include <GLFW/glfw3.h>
#include <core/Screen.h>
#include <data/Mesh.h>
#include <data/render/MeshRenderer.h>
#include <data/vk/Camera.h>
#include <map>
#include <core/ConfigRegistry.h>
#include <core/Utils.h>

extern ConfigRegistry registry;

extern std::vector<char> ReadFile(const std::string& filename);

void MeshRenderer::Render(RendererContext &context) {

    LightParameters ubo{};
    ubo.ambientStrength = 0.1f;
    ubo.specularStrength = 0.5f;
    ubo.colour = {1.0f, 1.0f, 1.0f, 1.0f};
    if(camera)
        ubo.viewPos = {camera->position.x, camera->position.y, camera->position.z, 1.0f};
    else
        ubo.viewPos = {0, 0, 0, 1.0f};
    ubo.lightPos = {1.2f, 1.0f, 2.0f, 1.0f};


    std::map<std::shared_ptr<ShaderProgram>, std::vector<std::shared_ptr<Mesh>>> meshMap;
    for (const std::shared_ptr<Mesh>& item : context.data.meshesToRender) {
        std::shared_ptr<ShaderProgram> key;
        if(context.data.GetKey(GLFW_KEY_3))
            key = this->geometryShader;
        else
            key = item->GetShaderProgram();
        meshMap[key].push_back(item);
    }

    int shaderKey = 0;
    int meshKey = 0;

    uint64_t disabledShaderMask = registry.Get<uint64_t>("renderer.mesh.disabledShaderMask", 0).ulong;
    uint64_t disabledMeshMask = registry.Get<uint64_t>("renderer.mesh.disabledMeshMask", 0).ulong;

    for(const auto& pair : meshMap) {

        // If bit is set, shader slot is disabled
        if(Utils::Bits::GetBit(disabledShaderMask, shaderKey))
            return;

        std::shared_ptr<ShaderProgram> shader;
        shader = pair.first;

        context.buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *shader->GetPipeline());
        shader->BindDescriptorSet(context.buffer, vk::PipelineBindPoint::eGraphics, context.bufferIdx);

        context.buffer.pushConstants(shader->GetLayout(), vk::ShaderStageFlagBits::eFragment, sizeof(MeshVertexPushConstants), sizeof(LightParameters), &ubo);

        for(const auto& mesh : pair.second) {

            // If bit is set, mesh slot is disabled
            if(Utils::Bits::GetBit(disabledMeshMask, meshKey))
                return;

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
            meshKey++;
        }
        shaderKey++;
    }
}

const char *MeshRenderer::Name() {
    return "Mesh Renderer";
}
void MeshRenderer::Setup(RendererSetupContext &context) {
    AbstractRenderer::Setup(context);
    std::vector<char> vertShaderCode = ReadFile("assets/shaders/geometry/geometry.vert");
    std::vector<char> fragShaderCode = ReadFile("assets/shaders/geometry/geometry.frag");

    geometryShader = context.core->CreateShaderProgram();
    geometryShader->AddStage(ShaderStage("main", vk::ShaderStageFlagBits::eVertex, vertShaderCode));
    geometryShader->AddStage(ShaderStage("main", vk::ShaderStageFlagBits::eFragment, fragShaderCode));
    geometryShader->AddPushConstant(0, sizeof(MeshVertexPushConstants), vk::ShaderStageFlagBits::eVertex);
    geometryShader->AddPushConstant(sizeof(MeshVertexPushConstants), sizeof(LightParameters), vk::ShaderStageFlagBits::eFragment);

    VertexDescription vertDesc;
    vertDesc.setInputRate(vk::VertexInputRate::eVertex);
    int stride = sizeof(Vertex);
    vertDesc.setStride(stride);
    vertDesc.Add(vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos));
    vertDesc.Add(vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal));
//    vertDesc.Add(vk::Format::eR32G32Sfloat, offsetof(Vertex, uv));

    geometryShader->AddVertexDescription(vertDesc);

    geometryShader->SetName("Geometry Shader");
    context.core->CompileShader(geometryShader);
}
void MeshRenderer::Dispose() {
    AbstractRenderer::Dispose();
    geometryShader->Cleanup();
}
void MeshRenderer::Resize(int width, int height) {
    AbstractRenderer::Resize(width, height);
    geometryShader->Recompile(setupContext.core);
}
