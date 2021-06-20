//
// Created by Guy on 03/06/2021.
//

#ifndef GLMODELVIEWER_MESH_H
#define GLMODELVIEWER_MESH_H

#include "Shader.h"
#include "ecs/Transform.h"
#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

class VulkanCore;

struct Vertex {
    glm::vec3 pos;
    glm::vec3 colour;
    glm::vec2 uv;

    Vertex(const glm::vec3 &pos, const glm::vec3 &colour, const glm::vec2 &uv) : pos(pos), colour(colour), uv(uv) {}

    static vk::VertexInputBindingDescription getBindingDescription() {
        vk::VertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;
        return bindingDescription;
    }

    static std::array<vk::VertexInputAttributeDescription, 3> getAttributeDescriptions() {
        std::array<vk::VertexInputAttributeDescription, 3> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[1].offset = offsetof(Vertex, colour);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = vk::Format::eR32G32Sfloat;
        attributeDescriptions[2].offset = offsetof(Vertex, uv);

        return attributeDescriptions;
    }
};

struct Triangle {
    uint32_t idxA;
    uint32_t idxB;
    uint32_t idxC;

    Triangle(uint32_t idxA, uint32_t idxB, uint32_t idxC) : idxA(idxA), idxB(idxB), idxC(idxC) {}

    static const int VERTEX_COUNT = 3;
};

class Mesh {
public:
    Mesh();
    void SetVertices(std::vector<Vertex> vertices);
    void SetIndices(std::vector<Triangle> indices);
    void Dispose();

    std::vector<Vertex> GetVertices();
    void CreateVertexBuffer(VulkanCore& core, vk::Device& device);
    void CreateIndexBuffer(VulkanCore& core, vk::Device& device);
    vk::Buffer GetVertexBuffer();
    uint32_t GetVertexCount();

    vk::Buffer GetIndexBuffer();
    uint32_t GetIndexCount();
    vk::IndexType GetIndexType();

    bool ShouldUseDefaultShader();
    std::shared_ptr<ShaderProgram> GetShaderProgram();
    void SetShaderProgram(std::shared_ptr<ShaderProgram> shader);

    glm::mat4 transform;
    glm::vec4 data;

    bool areBuffersValid = false;

protected:
    std::shared_ptr<ShaderProgram> shaderProgram;
    bool useDefaultShader;

    vk::Buffer vertexBuffer;
    vk::DeviceMemory bufferMemory;
    vk::Buffer indexBuffer;
    vk::DeviceMemory indexBufferMemory;
    vk::Device* devicePtr;
    std::vector<Vertex> vertices;
    std::vector<Triangle> indices;
    vk::IndexType indexType;

};

#endif//GLMODELVIEWER_MESH_H
