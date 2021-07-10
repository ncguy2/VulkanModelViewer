//
// Created by Guy on 01/07/2021.
//

#define NOMINMAX
#include <clocale>
#include <Plugin.h>
#include <loader/PMXLoader.h>
#include <mmd_pmx.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

std::vector<MeshData> PMXLoader::Load(FilePath& file) {
    std::vector<MeshData> data;

    KAITAI_INIT_READ(mmd_pmx_t, pmx, file);

    std::vector<Vertex> allVertices;
    std::vector<Triangle> allFaces;
    std::vector<std::string> textureNames(pmx->texture_count());

    for (const auto &item : *pmx->vertices()) {
        auto pos = item->position();
        auto nor = item->normal();
        auto uv = item->uv();

        allVertices.push_back(Vertex(
                {pos->x(), pos->y(), pos->z()},
                {nor->x(), nor->y(), nor->z()},
                {uv->x(), uv->y()}));
    }

    for (const auto &face : *pmx->faces()) {
        auto a = (*face->indices())[0];
        auto b = (*face->indices())[1];
        auto c = (*face->indices())[2];
        allFaces.emplace_back(a->value(), b->value(), c->value());
    }

    int id = 0;
    for (const auto &item : *pmx->textures()) {
        textureNames[id++] = item->name()->value();
    }

    int offset = 0;
    for (const auto &item : *pmx->materials()) {
        auto texName = textureNames[item->texture_index()->value()];
        int faces = item->face_vertex_count() / 3;
        int faceExtent = faces + offset;

        std::vector<Triangle> indices;
        unsigned int minIdx = allVertices.size();
        unsigned int maxIdx = 0;
        unsigned int min, max;
        for (unsigned int i = offset; i < faceExtent; i++) {
            min = allFaces[i].min();
            max = allFaces[i].max();

            if(min < minIdx)
                minIdx = min;
            if(max > maxIdx)
                maxIdx = max;

            indices.push_back(allFaces[i]);
        }

        // TODO Get range of vertices, offsetting the indices
//
//        std::vector<Vertex> verts(allVertices.begin() + minIdx, allVertices.begin() + maxIdx);
//        for (auto &item : indices)
//            item.offset(-minIdx);

        offset += faces;

        MeshData datum{};
        datum.texturePath = std::wstring((wchar_t*) texName.data(), texName.size()/2);
        datum.vertices = allVertices;
        datum.indices = indices;
        datum.transform = glm::translate(datum.transform, glm::vec3(0.0f, 0.0f, 0.0f));
//        datum.transform = glm::rotate(datum.transform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        datum.transform = glm::scale(datum.transform, glm::vec3(0.1f, 0.1f, 0.1f));

        data.push_back(datum);
    }

    return data;
}

PMXLoader::PMXLoader(Plugin *plugin) : ModelLoader(plugin) {}

void PMXLoader::LoadAsync(FilePath &filename, Delegate<MeshData&>::Signature callback) {
    KAITAI_INIT_READ(mmd_pmx_t, pmx, filename);

    std::vector<Vertex> allVertices;
    std::vector<Triangle> allFaces;
    std::vector<std::string> textureNames(pmx->texture_count());

    for (const auto &item : *pmx->vertices()) {
        auto pos = item->position();
        auto nor = item->normal();
        auto uv = item->uv();

        allVertices.push_back(Vertex(
                {pos->x(), pos->y(), pos->z()},
                {nor->x(), nor->y(), nor->z()},
                {uv->x(), uv->y()}));
    }

    for (const auto &face : *pmx->faces()) {
        auto a = (*face->indices())[0];
        auto b = (*face->indices())[1];
        auto c = (*face->indices())[2];
        allFaces.emplace_back(a->value(), b->value(), c->value());
    }

    unsigned int offset = 0;
    for (const auto &item : *pmx->materials()) {
        auto texName = (*pmx->textures())[item->texture_index()->value()]->name()->value();
        unsigned int faces = item->face_vertex_count() / 3;
        unsigned int faceExtent = faces + offset;

        std::vector<Triangle> indices;
        unsigned int minIdx = allVertices.size();
        unsigned int maxIdx = 0;
        
        unsigned int min, max;
        for (unsigned int i = offset; i < faceExtent; i++) {
            min = allFaces[i].min();
            max = allFaces[i].max();
            
            if(min < minIdx)
                minIdx = min;
            if(max > maxIdx)
                maxIdx = max;
            
            indices.push_back(allFaces[i]);
        }

        offset += faces;

        // TODO Get range of vertices, offsetting the indices

        std::vector<Vertex> verts(allVertices.begin() + minIdx, allVertices.begin() + maxIdx);
        for (auto &item : indices)
            item.offset(-minIdx);

        MeshData datum{};
        datum.texturePath = std::wstring((wchar_t *) texName.data(), texName.size() / 2);
        datum.vertices = verts;
        datum.indices = indices;
        datum.transform = glm::translate(datum.transform, glm::vec3(0.0f, 0.0f, -1.0f));
        datum.transform = glm::rotate(datum.transform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        datum.transform = glm::scale(datum.transform, glm::vec3(0.1f, 0.1f, -0.1f));

        callback(datum);
    }
}
