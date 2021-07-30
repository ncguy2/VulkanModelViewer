//
// Created by Guy on 01/07/2021.
//

#ifndef GLMODELVIEWER_PLUGIN_H
#define GLMODELVIEWER_PLUGIN_H

#define USE_PLUGIN using namespace Plugins;

#include <codecvt>
#include <filesystem>
#include <fstream>
#include <functional>
#include <utility>

#include "MeshData.h"

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)

#define PLUGIN_API __declspec(dllexport)

#elif defined(__APPLE__) || defined (__linux)

#define PLUGIN_API

#endif

#ifdef INCLUDE_KAITAI
#include <kaitai/kaitaistruct.h>

#define KAITAI_INIT(Type, variable) Type *variable = nullptr

#define KAITAI_READ(Type, variable, path)               \
    {                                                   \
        std::ifstream ifs(path, std::ifstream::binary); \
        kaitai::kstream ks(&ifs);                       \
        variable = new Type(&ks);                       \
    }

#define KAITAI_INIT_READ(Type, variable, path) KAITAI_INIT(Type, variable); KAITAI_READ(Type, variable, path);

#endif // INCLUDE_KAITAI

class VulkanCore;

namespace Plugins {

    using FilePath = std::wstring;

    class Plugin;

    class ModelLoader {
    public:
        explicit ModelLoader(Plugin *plugin) : plugin(plugin) {}
        virtual std::vector<MeshData> Load(FilePath& filename)=0;

    protected:
        Plugin* plugin;
    };

    class TextureLoader {
    public:
        explicit TextureLoader(Plugin *plugin) : plugin(plugin) {}
        virtual TextureData Load(FilePath& filename)=0;
        virtual void Free(TextureData& texData)=0;

    protected:
        Plugin* plugin;
    };

    /**
    * Plugins must expose a function with the signature `Plugin* InitialisePlugin()` to be imported
    * Destruction will be handled via the manager, by calling `Plugin::Dispose` before freeing the memory.
    * It's advised that no shared_ptrs to the plugin are kept by itself. (If they're needed, reset them in the dispose method)
    */
    class Plugin {
    public:
        virtual void Initialise()=0;
        virtual void Dispose()=0;
        virtual bool SupportsFileType(FileDataType dataType, FilePath& filename)=0;
        virtual ModelLoader* GetModelLoader(FilePath& filename)=0;
        virtual TextureLoader* GetTextureLoader(FilePath& filename)=0;

    };

};// namespace Plugins

#endif//GLMODELVIEWER_PLUGIN_H
