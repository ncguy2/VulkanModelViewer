//
// Created by Guy on 08/07/2021.
//

#include <data/VertexDescription.h>

void VertexDescription::setStride(int stride) {
    this->stride = stride;
}

void VertexDescription::setInputRate(vk::VertexInputRate inputRate) {
    this->inputRate = inputRate;
}

void VertexDescription::Add(vk::Format format, unsigned int offset) {
    attributes.emplace_back(format, offset);
}

VertexDescriptor VertexDescription::Convert(int binding) {
    VertexDescriptor desc{};

    desc.binding.binding = binding;
    desc.binding.stride = stride;
    desc.binding.inputRate = inputRate;

    desc.attrs.resize(attributes.size());
    for(int i = 0; i < attributes.size(); i++) {
        desc.attrs[i].binding = binding;
        desc.attrs[i].location = i;
        desc.attrs[i].format = attributes[i].format;
        desc.attrs[i].offset = attributes[i].offset;
    }

    return desc;
}
