//
// Created by Guy on 13/06/2021.
//

#ifndef GLMODELVIEWER_TEXTURESAMPLER_H
#define GLMODELVIEWER_TEXTURESAMPLER_H

#include <vulkan/vulkan.hpp>

class VulkanCore;

class TextureSampler {
public:
    explicit TextureSampler(VulkanCore* core, vk::Device& device);
    explicit TextureSampler(VulkanCore* core, vk::Device* device);
    void Create();
    void Dispose();

    vk::DescriptorImageInfo Prepare(vk::ImageView& view) const;

protected:
    VulkanCore* core;
    vk::Device* device;

    vk::Sampler sampler;
};

#endif//GLMODELVIEWER_TEXTURESAMPLER_H
