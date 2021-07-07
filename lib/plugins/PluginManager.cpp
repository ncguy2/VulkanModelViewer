//
// Created by Guy on 01/07/2021.
//

#include <plugins/PluginHost.h>
#include <plugins/PluginManager.h>
#include <iostream>
#include <cstdio>
#include <data/Texture.h>
#include <core/VulkanCore.hpp>

extern std::string ConvertWideToNormal(std::wstring wide);

void PluginManager::Initialise(VulkanCore* core) {
    this->core = core;
    LoadFromDirectory("plugins");
    LoadFromDirectory("plugins/PMX");
    LoadFromDirectory("plugins/StandardTextures");
}

void PluginManager::LoadFromDirectory(const char *directory) {
    std::filesystem::directory_iterator pluginDirectory(directory);

    for(auto& file : pluginDirectory) {
        std::filesystem::path path = file.path();
        std::cout << "Path: " << path.string() << std::endl;
        std::cout << " -- Extension: " << path.extension() << std::endl;

        if(!HasSuffix(path, LIBRARY_SUFFIX))
            continue;

        std::cout << " -- Is plugin" << std::endl;

        FilePath fp = path.wstring();

        std::shared_ptr<PluginHost> pluginHost = std::make_shared<PluginHost>(fp);
        pluginHost->Create();
        pluginHosts.push_back(pluginHost);
    }
}

void PluginManager::Dispose() {
    for (auto &item : pluginHosts)
        item->Dispose();
    pluginHosts.clear();
}

bool PluginManager::HasSuffix(std::filesystem::path path, const char *extension) {
    return path.extension() == extension;
}

bool PluginManager::SupportsFileType(FileDataType dataType, FilePath& filename) {
    for (auto &item : pluginHosts) {
        if(item->Get()->SupportsFileType(dataType, filename))
            return true;
    }
    return false;
}

ModelLoader *PluginManager::GetModelLoader(FilePath& filename) {
    for (auto &item : pluginHosts) {
        if(item->Get()->SupportsFileType(FileDataType::Model_Type, filename))
            return item->Get()->GetModelLoader(filename);
    }

    return nullptr;
}

TextureLoader *PluginManager::GetTextureLoader(FilePath& filename) {
    for (auto &item : pluginHosts) {
        if(item->Get()->SupportsFileType(FileDataType::Texture_Type, filename))
            return item->Get()->GetTextureLoader(filename);
    }

    return nullptr;
}

MeshData PluginManager::LoadMesh(FilePath& filename) {
    return LoadMeshes(filename)[0];
}

std::vector<MeshData> PluginManager::LoadMeshes(FilePath& filename) {
    ModelLoader* loader = GetModelLoader(filename);
    if(loader == nullptr)
        return std::vector<MeshData>();
    return loader->Load(filename);
}

std::shared_ptr<Texture> PluginManager::LoadTexture(FilePath& filename) {
    TextureLoader* loader = GetTextureLoader(filename);
    if(loader == nullptr) {
//        fprintf(stdout, "Unable to load texture at %ls\n", filename.wstring().c_str());
        std::cout << "Unable to load texture at " << CAST_WSTR_STR(filename) << std::endl;
        fflush(stdout);
        return defaultTexture;
    }

    TextureData data = loader->Load(filename);
    if(data.data == nullptr) {
        std::cout << "Failed to load " << CAST_WSTR_STR(filename) << std::endl;
        return defaultTexture;
    }

    std::shared_ptr<Texture> texPtr = core->CreateTexture(vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
    texPtr->SetName(CAST_WSTR_STR(filename));
    texPtr->SetSize(data.width, data.height);
    texPtr->Create(data.width * data.height * data.bytesPerPixel, data.data);
    loader->Free(data);
    return texPtr;
}

void PluginManager::SetDefaultTexture(std::shared_ptr<Texture> texture) {
    this->defaultTexture = texture;
}

bool PluginManager::IsDefaultTexture(std::shared_ptr<Texture> &tex) {
    return tex.get() == this->defaultTexture.get();
}

std::shared_ptr<Texture> PluginManager::GetDefaultTexture() {
    return this->defaultTexture;
}
