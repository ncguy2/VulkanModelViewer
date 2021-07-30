//
// Created by Guy on 01/07/2021.
//

#ifndef GLMODELVIEWER_MESHDATA_H
#define GLMODELVIEWER_MESHDATA_H

#define NOMINMAX

#include <vector>
#include <string>

#include <glm/glm.hpp>

typedef std::wstring TextureString;

struct Vertex {
    glm::vec3 pos{};
    glm::vec3 normal{};
    glm::vec2 uv{};

    Vertex() : pos({}), normal({}), uv({}) {}
    Vertex(const glm::vec3 &pos, const glm::vec3 &normal, const glm::vec2 &uv) : pos(pos), normal(normal), uv(uv) {}
};

struct Triangle {
    uint32_t idxA{};
    uint32_t idxB{};
    uint32_t idxC{};

    Triangle() : idxA(0), idxB(0), idxC(0) {}
    Triangle(uint32_t idxA, uint32_t idxB, uint32_t idxC) : idxA(idxA), idxB(idxB), idxC(idxC) {}

    static const int VERTEX_COUNT = 3;
    
    [[nodiscard]] uint32_t min() const {
        return std::min(idxA, std::min(idxB, idxC));
    }
    [[nodiscard]] uint32_t max() const {
        return std::max(idxA, std::max(idxB, idxC));
    }

    void offset(uint32_t offset) {
        idxA += offset;
        idxB += offset;
        idxC += offset;
    }
};

struct MeshData {
    glm::mat4 transform = glm::mat4(1.0f);
    std::vector<Vertex> vertices;
    std::vector<Triangle> indices;
    TextureString texturePath;
    TextureString meshPath;
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
