//
// Created by Guy on 21/07/2021.
//

#include <ObjLoader.h>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <tinyobjloader.h>

ObjLoader::ObjLoader(Plugin *plugin) : ModelLoader(plugin) {
}

std::vector<MeshData> ObjLoader::Load(FilePath &filePath) {

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
    std::string inputStr = convert.to_bytes(filePath);
    std::replace(inputStr.begin(), inputStr.end(), '\\', '/');

    tinyobj::ObjReaderConfig readerConfig;
    readerConfig.mtl_search_path = inputStr.substr(0, inputStr.rfind('/'));
    tinyobj::ObjReader reader;

    if(!reader.ParseFromFile(inputStr, readerConfig)) {
        if(!reader.Error().empty())
            std::cerr << "TinyObjReader error: " << reader.Error();
        return std::vector<MeshData>();
    }

    if(!reader.Warning().empty())
        std::cout << "TinyObjReader warning: " << reader.Warning();

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();


    std::vector<MeshData> meshes;

    for (size_t s = 0; s < shapes.size(); s++) {
        std::vector<Triangle> indices;
        std::vector<Vertex> allVertices;
        size_t index_offset = 0;
        for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);


            Triangle t{};

            for(size_t v = 0; v < fv; v++) {
                Vertex vert{};

                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                auto baseVertIdx = 3 * size_t(idx.vertex_index);
                vert.pos.x = attrib.vertices[baseVertIdx + 0];
                vert.pos.y = attrib.vertices[baseVertIdx + 1];
                vert.pos.z = attrib.vertices[baseVertIdx + 2];

                if(idx.normal_index >= 0) {
                    vert.normal.x = attrib.normals[3 * size_t(idx.normal_index) + 0];
                    vert.normal.y = attrib.normals[3 * size_t(idx.normal_index) + 1];
                    vert.normal.z = attrib.normals[3 * size_t(idx.normal_index) + 2];
                }

                if(idx.texcoord_index >= 0) {
                    vert.uv.x = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    vert.uv.y = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                }

                allVertices.push_back(vert);
                if(v % 3 == 0)
                    t.idxA = v + index_offset;
                else if(v % 3 == 1)
                    t.idxB = v + index_offset;
                else if(v % 3 == 2) {
                    t.idxC = v + index_offset;
                    indices.push_back(t);
                }
            }

            index_offset += fv;


        }

        MeshData meshData{};

        meshData.vertices = allVertices;
        meshData.indices = indices;
        meshData.meshPath = filePath;
        meshData.texturePath = L"";
        meshData.transform = glm::scale(meshData.transform, glm::vec3(0.01f, 0.01f, 0.01f));

        meshes.push_back(meshData);

    }


    return meshes;
}
