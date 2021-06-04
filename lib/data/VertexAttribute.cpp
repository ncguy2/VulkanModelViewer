//
// Created by Guy on 04/06/2021.
//

#include <data/VertexAttribute.h>

VertexAttribute::VertexAttribute(const char *name, int type, int normalised, int count, int typeSize)
 : name(name), type(type), normalised(normalised), count(count), typeSize(typeSize) {
}

namespace VertexAttributes {
    VertexAttribute Vertex("VERTEX", GL_FLOAT, GL_FALSE, 3, sizeof(float));
    VertexAttribute Normal("NORMAL", GL_FLOAT, GL_FALSE, 3, sizeof(float));
    VertexAttribute Colour("COLOUR", GL_FLOAT, GL_FALSE, 3, sizeof(float));
    VertexAttribute TexCoords("TexCoords", GL_FLOAT, GL_FALSE, 2, sizeof(float));
};