//
// Created by Guy on 01/07/2021.
//

#include <codecvt>
#include <clocale>
#include <cuchar>
#include <Plugin.h>
#include <loader/PMXLoader.h>
#include <mmd_pmx.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#define UNICODE
#include <windows.h>

TextureString ReadMultibyteString(std::string mbs) {
    TextureString output{};

    for(int i = 0; i < mbs.size(); i++) {
        auto item = mbs[i];
        if((item & 0b10000000) == 0) { // ASCII
            output.push_back(item);
            continue;
        }

        unsigned int c;

        if((item & 0b11000000) == 0b11000000) { // Header byte
            c = (unsigned char) item;
        }

        while((mbs[i + 1] & 0b11000000) == 0b10000000) { // Continuation byte
            c = (c << 8) | (unsigned char) mbs[i + 1];
            i++;
        }

        output.push_back((char32_t) c);
    }

    return output;
}

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
//        std::cout << "Texture: " << item->name()->value() << std::endl;
    }

    int offset = 0;
    for (const auto &item : *pmx->materials()) {
        auto texName = textureNames[item->texture_index()->value()];
        int faces = item->face_vertex_count() / 3;
        int faceExtent = faces + offset;

        std::vector<Triangle> indices;
        for(int i = offset; i < faceExtent; i++) {
            indices.push_back(allFaces[i]);
        }

        offset += faces;

//        std::wstring s((wchar_t*) texName.data(), texName.size()/2);

//        int len;
//        int slength = (int)s.length() + 1;
//        len = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
//        char* buf = new char[len];
//        WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, buf, len, 0, 0);
//        std::string r(buf);
//        delete[] buf;

        MeshData datum{};
        datum.texturePath = std::wstring((wchar_t*) texName.data(), texName.size()/2);
        datum.vertices = allVertices;
        datum.indices = indices;
        datum.transform = glm::translate(datum.transform, glm::vec3(0.0f, 0.0f, -1.0f));
        datum.transform = glm::rotate(datum.transform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        datum.transform = glm::scale(datum.transform, glm::vec3(0.1f, 0.1f, 0.1f));

        data.push_back(datum);
    }

    return data;
}

PMXLoader::PMXLoader(Plugin *plugin) : ModelLoader(plugin) {}
