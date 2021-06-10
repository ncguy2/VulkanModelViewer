//
// Created by Guy on 03/06/2021.
//

#ifndef GLMODELVIEWER_MESH_H
#define GLMODELVIEWER_MESH_H

#include <vector>
#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

class VulkanCore;

struct Vertex {
    glm::vec3 pos;
    glm::vec3 colour;

    Vertex(const glm::vec3 &pos, const glm::vec3 &colour) : pos(pos), colour(colour) {

    }

    static vk::VertexInputBindingDescription getBindingDescription() {
        vk::VertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;
        return bindingDescription;
    }

    static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
        attributeDescriptions[1].offset = offsetof(Vertex, colour);

        return attributeDescriptions;
    }
};

class Mesh {
public:
    void SetVertices(std::vector<Vertex> vertices);
    void Dispose();

    std::vector<Vertex> GetVertices();
    void CreateVertexBuffer(VulkanCore* core, vk::Device* device);
    vk::Buffer GetVertexBuffer();
    uint32_t GetVertexCount();

protected:
    vk::Buffer vertexBuffer;
    vk::Device* devicePtr;
    std::vector<Vertex> vertices;
    vk::DeviceMemory bufferMemory;
};

#endif//GLMODELVIEWER_MESH_H
