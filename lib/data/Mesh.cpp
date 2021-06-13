//
// Created by Guy on 03/06/2021.
//

#include <data/Mesh.h>
#include <core/VulkanCore.hpp>

Mesh::Mesh() : indexType(vk::IndexType::eUint32), shaderProgram(nullptr), useDefaultShader(true) {}

void Mesh::SetVertices(std::vector<Vertex> vertices) {
    this->vertices = vertices;
}

void Mesh::SetIndices(std::vector<Triangle> indices) {
    this->indices = indices;
}

std::vector<Vertex> Mesh::GetVertices() {
    return this->vertices;
}

void Mesh::CreateVertexBuffer(VulkanCore& core, vk::Device& device) {
    devicePtr = &device;
    vk::DeviceSize size = sizeof(vertices[0]) * vertices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;

    core.CreateBuffer(size, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

    void* data;
    if(device.mapMemory(stagingBufferMemory, 0, size, (vk::MemoryMapFlags) 0, &data) != vk::Result::eSuccess)
        throw std::runtime_error("Unable to map device memory");
    memcpy(data, vertices.data(), size);
    device.unmapMemory(stagingBufferMemory);

    core.CreateBuffer(size, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vertexBuffer, bufferMemory);
    core.CopyBuffer(stagingBuffer, vertexBuffer, size);
    core.DestroyBuffer(stagingBuffer, stagingBufferMemory);
}

void Mesh::CreateIndexBuffer(VulkanCore& core, vk::Device& device) {
    vk::DeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;

    core.CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

    void* data;
    if(device.mapMemory(stagingBufferMemory, 0, bufferSize, (vk::MemoryMapFlags) 0, &data) != vk::Result::eSuccess)
        throw std::runtime_error("Unable to map device memory");
    memcpy(data, indices.data(), bufferSize);
    device.unmapMemory(stagingBufferMemory);

    core.CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, indexBuffer, indexBufferMemory);
    core.CopyBuffer(stagingBuffer, indexBuffer, bufferSize);
    core.DestroyBuffer(stagingBuffer, stagingBufferMemory);
}

vk::Buffer Mesh::GetVertexBuffer() {
    return vertexBuffer;
}

uint32_t Mesh::GetVertexCount() {
    return vertices.size();
}

vk::Buffer Mesh::GetIndexBuffer() {
    return indexBuffer;
}

uint32_t Mesh::GetIndexCount() {
    return indices.size() * Triangle::VERTEX_COUNT;
}

vk::IndexType Mesh::GetIndexType() {
    return indexType;
}

bool Mesh::ShouldUseDefaultShader() {
    return useDefaultShader || shaderProgram == nullptr;
}
std::shared_ptr<ShaderProgram> Mesh::GetShaderProgram() {
    if(this->ShouldUseDefaultShader())
        return nullptr;
    return shaderProgram;
}
void Mesh::SetShaderProgram(std::shared_ptr<ShaderProgram> shader) {
    shaderProgram = std::move(shader);
    useDefaultShader = false;
}

void Mesh::Dispose() {
    if(devicePtr) {
        devicePtr->destroyBuffer(vertexBuffer);
        devicePtr->destroyBuffer(indexBuffer);
        devicePtr->freeMemory(bufferMemory);
        devicePtr->freeMemory(indexBufferMemory);
    }
}
