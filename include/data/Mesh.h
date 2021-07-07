//
// Created by Guy on 03/06/2021.
//

#ifndef GLMODELVIEWER_MESH_H
#define GLMODELVIEWER_MESH_H

#include "MeshData.h"

#include "Shader.h"
#include "ecs/Transform.h"
#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.hpp>

class VulkanCore;

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
