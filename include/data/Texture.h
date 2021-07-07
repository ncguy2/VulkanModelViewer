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

    void SetName(std::wstring name);
    void SetName(std::string name);
    void LoadFromFile(const char* file, bool hasAlpha = true);
    void Create(int size, void* data = nullptr);
    void Set(vk::Image& image);
    void Dispose();
    void Transition(vk::ImageLayout oldLayout, vk::ImageLayout newLayout);

    void SetSize(int width, int height);

    vk::ImageView GetView();
    vk::ImageView *GetViewPtr();

    vk::Image& GetImage();

    vk::ImageUsageFlags usageFlags;
    vk::Format format;

protected:
    void CreateView();

    std::string name;
    vk::Device* device;
    VulkanCore* core;

    int width;
    int height;
    int texChannels;

    vk::DeviceSize imageSize;
    vk::Image textureImage;
    vk::DeviceMemory textureImageMemory;
    vk::ImageView textureView;
    vk::ImageAspectFlags aspectFlags;
    bool manageMemory = true;
};

#endif//GLMODELVIEWER_TEXTURE_H
