//
// Created by Guy on 01/07/2021.
//

#include <plugins/PluginHost.h>
#include <plugins/PluginManager.h>
#include <iostream>
#include <cstdio>
#include <data/Texture.h>
#include <core/VulkanCore.hpp>
#include <Logging.h>
#include <sys/stat.h>

extern std::string ConvertWideToNormal(std::wstring wide);
extern std::string assetRoot;

inline bool exists(const std::string& name) {
    FILE* file;
    fopen_s(&file, name.c_str(), "rb");
    if(file) {
        fclose(file);
        Logging::Log("File exists");
        return true;
    }
    Logging::Log("File does not exist");
    return false;
}

inline bool exists(const std::wstring& name) {
    FILE* file;
    _wfopen_s(&file, name.c_str(), L"rb");
    if(file) {
        fclose(file);
        Logging::Log("File exists");
        return true;
    }
    Logging::Log("File does not exist");
    return false;
}

void PluginManager::Initialise(VulkanCore* core) {
    this->core = core;
    LoadFromDirectory("plugins");
    LoadFromDirectory("plugins/PMX");
    LoadFromDirectory("plugins/StandardTextures");
}

void PluginManager::LoadFromDirectory(const char *directory) {
    std::string p = assetRoot + directory;

    if(!std::filesystem::exists(p)) {
        LOG_LN("Plugin directory \"" + p + "\" doesn't exist")
        return;
    }

    LOG("Loading from directory " + p);
    std::filesystem::directory_iterator pluginDirectory(p);

    for(auto& file : pluginDirectory) {
        std::filesystem::path path = file.path();
        Logging::Get() << "Path: " << path.string() << std::endl;
        Logging::Get() << " -- Extension: " << path.extension() << std::endl;

        if(!HasSuffix(path, LIBRARY_SUFFIX))
            continue;

        Logging::Get() << " -- Is plugin" << std::endl;

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
    if(!exists(filename))
        return false;

    for (auto &item : pluginHosts) {
        if(item->Get()->SupportsFileType(dataType, filename))
            return true;
    }
    return false;
}

ModelLoader *PluginManager::GetModelLoader(FilePath& filename) {
    if(!exists(filename))
        return nullptr;

    for (auto &item : pluginHosts) {
        if(item->Get()->SupportsFileType(FileDataType::Model_Type, filename))
            return item->Get()->GetModelLoader(filename);
    }

    return nullptr;
}

TextureLoader *PluginManager::GetTextureLoader(FilePath& filename) {
    if(!exists(filename))
        return nullptr;

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
    LOG_LN("ModelLoader instance found");
    auto data = loader->Load(filename);
    LOG_LN("ModelLoader instance able to load from file");
    for (auto &item : data)
        item.meshPath = filename;
    return data;
}

std::shared_ptr<Texture> PluginManager::LoadTexture(FilePath& filename) {
    TextureLoader* loader = GetTextureLoader(filename);
    if(loader == nullptr) {
//        fprintf(stdout, "Unable to load texture at %ls\n", filename.wstring().c_str());
        Logging::Get() << "Unable to load texture at " << CAST_WSTR_STR(filename) << std::endl;
        fflush(stdout);
        return defaultTexture;
    }

    TextureData data = loader->Load(filename);
    if(data.data == nullptr) {
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

void PluginManager::LoadMeshesAsync(FilePath &filename, PluginManager::MeshAsync::Signature callback) {
    ModelLoader* loader = GetModelLoader(filename);
    if(loader == nullptr)
        return;
    loader->LoadAsync(filename, callback);
}
