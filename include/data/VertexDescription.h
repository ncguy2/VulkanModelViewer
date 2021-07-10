//
// Created by Guy on 08/07/2021.
//

#ifndef GLMODELVIEWER_VERTEXDESCRIPTION_H
#define GLMODELVIEWER_VERTEXDESCRIPTION_H

#include <vulkan/vulkan.hpp>

struct VertexDescriptionAttribute {
    vk::Format format;
    unsigned int offset;
};

struct VertexDescriptor {
    vk::VertexInputBindingDescription binding;
    std::vector<vk::VertexInputAttributeDescription> attrs;
};

class VertexDescription {
public:
    void Add(vk::Format format, unsigned int offset);

    void setStride(int stride);
    void setInputRate(vk::VertexInputRate inputRate);

    VertexDescriptor Convert(int binding);

protected:
    int stride;
    vk::VertexInputRate inputRate = vk::VertexInputRate::eVertex;
    std::vector<VertexDescriptionAttribute> attributes;
};

#endif//GLMODELVIEWER_VERTEXDESCRIPTION_H
