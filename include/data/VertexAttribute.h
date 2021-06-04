//
// Created by Guy on 04/06/2021.
//

#ifndef GLMODELVIEWER_VERTEXATTRIBUTE_H
#define GLMODELVIEWER_VERTEXATTRIBUTE_H

#include <glad/glad.h>
class VertexAttribute {
public:
    VertexAttribute(const char *name, int type, int normalised, int count, int typeSize);

    const char* name;
    int type;
    int normalised;
    int count;
    int typeSize;
};

#endif//GLMODELVIEWER_VERTEXATTRIBUTE_H
