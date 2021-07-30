//
// Created by Guy on 01/07/2021.
//

#ifndef GLMODELVIEWER_PLUGINMANAGER_H
#define GLMODELVIEWER_PLUGINMANAGER_H

#include <core/Events.h>
#include <filesystem>
#include <memory>
#include <plugins/api/MeshData.h>
#include <plugins/api/Plugin.h>
#include <vector>

//namespace Plugins {
//    class ModelLoader;
//    class TextureLoader;
//};

using FilePath = Plugins::FilePath;

class Texture;
class PluginHost;
class VulkanCore;

class PluginManager {
public:

    typedef Delegate<MeshData&> MeshAsync;

    void Initialise(VulkanCore* core);
    void Dispose();

    bool SupportsFileType(FileDataType dataType, FilePath& filename);
    Plugins::ModelLoader* GetModelLoader(FilePath& filename);
    Plugins::TextureLoader* GetTextureLoader(FilePath& filename);
    MeshData LoadMesh(FilePath& filename);
    std::vector<MeshData> LoadMeshes(FilePath& filename);
    std::shared_ptr<Texture> LoadTexture(FilePath& filename);

    void SetDefaultTexture(std::shared_ptr<Texture> texture);
    bool IsDefaultTexture(std::shared_ptr<Texture> &type);
    std::shared_ptr<Texture> GetDefaultTexture();

private:
    void LoadFromDirectory(const char* directory, int maxDepth = 1);
    bool HasSuffix(std::filesystem::path path, const char* extension);

    std::shared_ptr<Texture> defaultTexture;
    std::vector<std::shared_ptr<PluginHost>> pluginHosts;
    VulkanCore* core;
};

#endif//GLMODELVIEWER_PLUGINMANAGER_H
