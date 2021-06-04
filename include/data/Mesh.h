//
// Created by Guy on 03/06/2021.
//

#ifndef GLMODELVIEWER_MESH_H
#define GLMODELVIEWER_MESH_H

#include "VertexAttribute.h"
#include <vector>

class Mesh {
public:
    virtual ~Mesh();

    void AddAttribute(VertexAttribute attr);

    void SetVertices(std::vector<float> data);
    void SetIndices(std::vector<unsigned int> data);

    void SetVertices(float* data, unsigned int count);
    void SetIndices(unsigned int* data, unsigned int count);
    void Initialize();
    void Bind();

    void Draw();

protected:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    float* Vertices;
    unsigned int VertexCount;

    unsigned int* Indices;
    unsigned int IndexCount;

    std::vector<VertexAttribute> Attributes;
};

#endif//GLMODELVIEWER_MESH_H
