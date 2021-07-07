//
// Created by Guy on 01/07/2021.
//

#ifndef GLMODELVIEWER_MESHDATA_H
#define GLMODELVIEWER_MESHDATA_H

#include <vector>
#include <string>

#ifdef USE_VK_MESH
#include <vulkan/vulkan.hpp>
#endif // USE_VK_MESH
#include <glm/glm.hpp>

typedef std::wstring TextureString;

struct Vertex {
    glm::vec3 pos{};
    glm::vec3 normal{};
    glm::vec2 uv{};

    Vertex(const glm::vec3 &pos, const glm::vec3 &normal, const glm::vec2 &uv) : pos(pos), normal(normal), uv(uv) {}

#ifdef USE_VK_MESH
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
        attributeDescriptions[1].offset = offsetof(Vertex, normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = vk::Format::eR32G32Sfloat;
        attributeDescriptions[2].offset = offsetof(Vertex, uv);

        return attributeDescriptions;
    }
#endif // USE_VK_MESH
};

struct Triangle {
    uint32_t idxA{};
    uint32_t idxB{};
    uint32_t idxC{};

    Triangle(uint32_t idxA, uint32_t idxB, uint32_t idxC) : idxA(idxA), idxB(idxB), idxC(idxC) {}

    static const int VERTEX_COUNT = 3;
};

struct MeshData {
    glm::mat4 transform = glm::mat4(1.0f);
    std::vector<Vertex> vertices;
    std::vector<Triangle> indices;
    TextureString texturePath;
};

struct TextureData {
//    vk::Format format;
//    vk::ImageAspectFlags aspectFlags;
    int width;
    int height;
    int bytesPerPixel;
    void* data;
};

enum FileDataType {
    Model_Type,
    Texture_Type
};

#endif//GLMODELVIEWER_MESHDATA_H
