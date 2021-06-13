//
// Created by Guy on 12/06/2021.
//

#ifndef GLMODELVIEWER_TEXTURE_H
#define GLMODELVIEWER_TEXTURE_H

#include <vulkan/vulkan.hpp>

class VulkanCore;

class Texture {
public:
    Texture(VulkanCore* core, vk::Device& device, vk::Format format, vk::ImageAspectFlags aspectFlags);
    ~Texture();

    void LoadFromFile(const char* file, bool hasAlpha = true);
    void Create(int size, void* data = nullptr);

    void SetSize(int width, int height);

    vk::ImageView GetView();
    vk::Image& GetImage();

    vk::ImageUsageFlags usageFlags;

protected:
    void CreateView();

    vk::Device* device;
    VulkanCore* core;

    int width;
    int height;
    int texChannels;

    vk::DeviceSize imageSize;
    vk::Format format;
    vk::Image textureImage;
    vk::DeviceMemory textureImageMemory;
    vk::ImageView textureView;
    vk::ImageAspectFlags aspectFlags;
};

#endif//GLMODELVIEWER_TEXTURE_H
