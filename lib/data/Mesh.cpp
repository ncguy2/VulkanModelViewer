//
// Created by Guy on 03/06/2021.
//

#include <data/Mesh.h>
#include <glad/glad.h>
#include <iostream>
#include <string>
#include <utility>

void Mesh::Initialize() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * VertexCount, Vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * IndexCount, Indices, GL_STATIC_DRAW);

    unsigned int id = 0;
    unsigned int offset = 0;
    int size;

    int stride = 0;
    for (const VertexAttribute &attr : Attributes) {
        stride += attr.count * attr.typeSize;
    }

    for (const VertexAttribute &attr : Attributes) {
        std::cout << attr.name << " at offset " << std::to_string(offset) << std::endl;
        glVertexAttribPointer(id, attr.count, attr.type, attr.normalised, stride, (void*) offset);
        glEnableVertexAttribArray(id);

        offset += attr.count * attr.typeSize;
        id++;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::SetVertices(float* data, unsigned int count) {
    this->Vertices = data;
    this->VertexCount = count;
}

void Mesh::SetIndices(unsigned int *data, unsigned int count) {
    this->Indices = data;
    this->IndexCount = count;
}

void Mesh::Bind() {
    glBindVertexArray(VAO);
}

void Mesh::Draw() {
    glBindVertexArray(VAO);
//    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, IndexCount, GL_UNSIGNED_INT, 0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    delete this->Vertices;
}

void Mesh::SetVertices(std::vector<float> data) {
    this->VertexCount = data.size();
    this->Vertices = new float[VertexCount];
    std::copy(data.begin(), data.end(), this->Vertices);
}
void Mesh::SetIndices(std::vector<unsigned int> data) {
    this->IndexCount = data.size();
    this->Indices = new unsigned int[IndexCount];
    std::copy(data.begin(), data.end(), this->Indices);
}
void Mesh::AddAttribute(VertexAttribute attr) {
    Attributes.push_back(attr);
}
