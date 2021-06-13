//
// Created by Guy on 12/06/2021.
//

#include <pch.h>

#include <core/VulkanCore.hpp>
#include <data/Texture.h>

#include <stb_image.h>

Texture::Texture(VulkanCore* core, vk::Device& device, vk::Format format, vk::ImageAspectFlags aspectFlags) : core(core), device(&device), format(format), aspectFlags(aspectFlags) {}

void Texture::LoadFromFile(const char *file, bool hasAlpha) {
    stbi_uc* pixels = stbi_load(file, &width, &height, &texChannels, hasAlpha ? STBI_rgb_alpha : STBI_rgb);

    if(!pixels)
        throw std::runtime_error("Failed to load texture image");

    Create(width * height * 4, pixels);
    stbi_image_free(pixels);
}

void Texture::Create(int size, void *pixels) {
    imageSize = size;
    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;

    core->CreateBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

    if(pixels != nullptr) {
        void *data;
        CHECK(device->mapMemory(stagingBufferMemory, 0, imageSize, (vk::MemoryMapFlags) 0, &data));
        memcpy(data, pixels, imageSize);
        device->unmapMemory(stagingBufferMemory);
    }

    vk::ImageCreateInfo imageInfo{};
    imageInfo.setImageType(vk::ImageType::e2D);
    imageInfo.extent.setWidth(width).setHeight(height).setDepth(1);
    imageInfo.setMipLevels(1);
    imageInfo.setArrayLayers(1);
    imageInfo.setFormat(format);
    imageInfo.setTiling(vk::ImageTiling::eOptimal);
    imageInfo.setInitialLayout(vk::ImageLayout::eUndefined);
    imageInfo.setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled | usageFlags);
    imageInfo.setSharingMode(vk::SharingMode::eExclusive);
//    imageInfo.setFlags(0);

    CHECK(device->createImage(&imageInfo, nullptr, &textureImage));

    vk::MemoryRequirements memoryRequirements;
    device->getImageMemoryRequirements(textureImage, &memoryRequirements);

    vk::MemoryAllocateInfo allocInfo{};
    allocInfo.setAllocationSize(memoryRequirements.size);
    allocInfo.setMemoryTypeIndex(core->FindMemoryType(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal));

    CHECK(device->allocateMemory(&allocInfo, nullptr, &textureImageMemory));
    device->bindImageMemory(textureImage, textureImageMemory, 0);

    if(pixels != nullptr) {
        core->TransitionImageLayout(textureImage, format, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
        core->CopyBufferToImage(stagingBuffer, textureImage, width, height);
        core->TransitionImageLayout(textureImage, format, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
    }

    device->destroyBuffer(stagingBuffer);
    device->freeMemory(stagingBufferMemory);

    CreateView();
}

void Texture::CreateView() {
    textureView = core->CreateImageView(textureImage, format, aspectFlags);
}

Texture::~Texture() {
    device->destroyImageView(textureView);
    device->destroyImage(textureImage);
    device->freeMemory(textureImageMemory);
}
vk::ImageView Texture::GetView() {
    return textureView;
}

vk::Image& Texture::GetImage() {
    return textureImage;
}

void Texture::SetSize(int width, int height) {
    this->width = width;
    this->height = height;
}
