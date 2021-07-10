//
// Created by Guy on 03/07/2021.
//

#include <windows.h>
#include <StdTexLoader.h>
#include <gif.h>
#include <stb_image.h>
#include <codecvt>
#include <iostream>

namespace Kaitai {
    template <typename T>
    extern T Read(const FilePath& path);
}

TextureData StdTexLoader::Load(FilePath& filename) {

    if(cache.contains(filename))
        return cache[filename];

    int width;
    int height;
    int texChannels;
    bool hasAlpha = true;

    FILE *f = nullptr;
    _wfopen_s(&f, filename.c_str(), L"rb");

    if(!f) {
        return TextureData{
            1, 1, 1, nullptr
        };
    }

    stbi_uc *pixels;
    pixels = stbi_load_from_file(f, &width, &height, &texChannels, STBI_rgb + hasAlpha);
    fclose(f);

//    stbi_uc* pixels = stbi_load(filename, &width, &height, &texChannels, STBI_rgb + hasAlpha);

    if(!pixels)
        throw std::runtime_error("Failed to load texture image");

    TextureData data{};
    data.width = width;
    data.height = height;
    data.bytesPerPixel = 4;
    data.data = pixels;

    cache.try_emplace(filename, data);

    return data;
}

void StdTexLoader::Free(TextureData &texData) {
//    stbi_image_free(texData.data);
}

StdTexLoader::StdTexLoader(Plugin *plugin) : TextureLoader(plugin) {}

StdTexLoader::~StdTexLoader() {
    for (auto &item : cache)
        stbi_image_free(item.second.data);
}
