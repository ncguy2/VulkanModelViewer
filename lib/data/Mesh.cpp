//
// Created by Guy on 03/06/2021.
//

#include <data/Mesh.h>
#include <core/VulkanCore.hpp>

void Mesh::SetVertices(std::vector<Vertex> vertices) {
    this->vertices = vertices;
}

std::vector<Vertex> Mesh::GetVertices() {
    return this->vertices;
}

void Mesh::CreateVertexBuffer(VulkanCore* core, vk::Device* device) {
    devicePtr = device;

    vk::BufferCreateInfo bufferInfo{};
    bufferInfo.size = sizeof(vertices[0]) * vertices.size();
    bufferInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    if(device->createBuffer(&bufferInfo, nullptr, &vertexBuffer) != vk::Result::eSuccess)
        throw std::runtime_error("Unable to create vertex buffer");

    core->AllocateMemory(vertexBuffer, &bufferMemory);

    void* data;
    if(device->mapMemory(bufferMemory, 0, bufferInfo.size, (vk::MemoryMapFlags) 0, &data) != vk::Result::eSuccess)
        throw std::runtime_error("Unable to map devicememory");
    memcpy(data, vertices.data(), bufferInfo.size);
    device->unmapMemory(bufferMemory);
}

vk::Buffer Mesh::GetVertexBuffer() {
    return vertexBuffer;
}

uint32_t Mesh::GetVertexCount() {
    return vertices.size();
}

void Mesh::Dispose() {
    if(devicePtr) {
        devicePtr->destroyBuffer(vertexBuffer);
        devicePtr->freeMemory(bufferMemory);
    }
}
