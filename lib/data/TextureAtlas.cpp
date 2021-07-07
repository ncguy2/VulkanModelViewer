//
// Created by Guy on 06/07/2021.
//

#include <data/TextureAtlas.h>
#include <core/VulkanCore.hpp>

TextureAtlas::TextureAtlas(VulkanCore *core, vk::Device* device) : core(core), device(device) {}

void TextureAtlas::Initialise(vk::Format format, vk::ImageAspectFlags aspect, int cellCountX, int cellCountY, int cellWidth, int cellHeight) {
    this->cellCountX = cellCountX;
    this->cellCountY = cellCountY;
    this->cellWidth = cellWidth;
    this->cellHeight = cellHeight;

    this->textureWidth = cellCountX * cellWidth;
    this->textureHeight = cellCountY * cellHeight;

    actualTexture = core->CreateTexture(format, aspect);
    static int textureAtlasCounter = 0;
    actualTexture->SetName("Texture Atlas " + std::to_string(textureAtlasCounter++));
    actualTexture->SetSize(textureWidth, textureHeight);

    channels = 4;
    switch(format) {
        case vk::Format::eR8Srgb:
            channels = 1;
            break;
        case vk::Format::eR8G8Srgb:
            channels = 3;
            break;
        case vk::Format::eR8G8B8Srgb:
            channels = 3;
            break;
        case vk::Format::eR8G8B8A8Srgb:
            channels = 4;
            break;
    }

    actualTexture->Create(this->textureWidth * this->textureHeight * channels);
}

int TextureAtlas::AddTexture(void *pixels) {
    int stagingBufferSize = this->cellWidth * this->cellHeight * this->channels;
    vk::Buffer stagingBuffer;
    vk::DeviceMemory stagingBufferMemory;

    core->CreateBuffer(stagingBufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

    void *data;
    CHECK(device->mapMemory(stagingBufferMemory, 0, stagingBufferSize, (vk::MemoryMapFlags) 0, &data));
    memcpy(data, pixels, stagingBufferSize);
    device->unmapMemory(stagingBufferMemory);

    glm::ivec2 texCoords = SlotToTextureCoords(currentSlot);
    currentSlot++;

    core->CopyBufferToImage(stagingBuffer, actualTexture->GetImage(), this->cellWidth, this->cellHeight, texCoords.x, texCoords.y);

    device->destroyBuffer(stagingBuffer);
    device->freeMemory(stagingBufferMemory);

    return currentSlot;
}

int TextureAtlas::TextureCoordsToSlot(int x, int y) {
    int a = x / cellWidth;
    int b = y / cellHeight;

    return (b * cellCountX) + a;
}

glm::ivec2 TextureAtlas::SlotToTextureCoords(int slot) {
    int x = slot % cellWidth;
    int y = slot / cellWidth;

    return glm::ivec2(x, y);
}
