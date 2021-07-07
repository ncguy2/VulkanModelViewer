//
// Created by Guy on 06/07/2021.
//

#ifndef GLMODELVIEWER_TEXTUREATLAS_H
#define GLMODELVIEWER_TEXTUREATLAS_H

#include "Texture.h"
#include <glm/glm.hpp>

class VulkanCore;

class TextureAtlas {
public:
    explicit TextureAtlas(VulkanCore* core, vk::Device* device);

    void Initialise(vk::Format format, vk::ImageAspectFlags aspect, int cellCountX, int cellCountY, int cellWidth, int cellHeight);
    int AddTexture(void* pixels);

protected:

    int cellCountX;
    int cellCountY;
    int cellWidth;
    int cellHeight;
    int channels;
    int textureWidth;
    int textureHeight;

    int currentSlot = 0;


    int TextureCoordsToSlot(int x, int y);
    glm::ivec2 SlotToTextureCoords(int slot);

    std::shared_ptr<Texture> actualTexture;

    VulkanCore* core;
    vk::Device* device;
};

#endif//GLMODELVIEWER_TEXTUREATLAS_H
