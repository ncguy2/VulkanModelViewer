param(
    [string]$PluginName,
    [string]$Directory = "./$PluginName",
    [string]$SourceTree = "",
    [string]$glmPath = "",
    [switch]$initCmake,
    [switch]$useNinja
)

$UseSourceTree = $SourceTree.Length -gt 0
$UseLocalGLM = $glmPath.Length -gt 0

$dollar = '$'

$cmakeLists = @"
cmake_minimum_required(VERSION 3.18)
project(${PluginName}Plugin)

set(CMAKE_CXX_STANDARD 20)

set(${PluginName}Plugin_HDRS include/${PluginName}Plugin.h)
set(${PluginName}Plugin_SRCS lib/${PluginName}Plugin.cpp)

if(EXISTS "gen")
    file(GLOB GEN_SRCS gen/*.cpp gen/*.h )
else()
    SET(GEN_SRCS )
endif()

add_library(${PluginName}Plugin SHARED ${dollar}{${PluginName}Plugin_HDRS} ${dollar}{${PluginName}Plugin_SRCS} ${dollar}{GEN_SRCS})
target_include_directories(${PluginName}Plugin PRIVATE include)

"@

$plugin_h = @"
#ifndef GLMODELVIEWER_PLUGIN_H
#define GLMODELVIEWER_PLUGIN_H

#define USE_PLUGIN using namespace Plugins;

#include <codecvt>
#include <filesystem>
#include <fstream>
#include <functional>
#include "MeshData.h"
#include <utility>

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
"@

$meshdata_h = @"
#ifndef GLMODELVIEWER_MESHDATA_H
#define GLMODELVIEWER_MESHDATA_H

#define NOMINMAX

#include <vector>
#include <string>

#include <glm/glm.hpp>

typedef std::wstring TextureString;

struct Vertex {
    glm::vec3 pos{};
    glm::vec3 normal{};
    glm::vec2 uv{};

    Vertex(const glm::vec3 &pos, const glm::vec3 &normal, const glm::vec2 &uv) : pos(pos), normal(normal), uv(uv) {}
};

struct Triangle {
    uint32_t idxA{};
    uint32_t idxB{};
    uint32_t idxC{};

    Triangle(uint32_t idxA, uint32_t idxB, uint32_t idxC) : idxA(idxA), idxB(idxB), idxC(idxC) {}

    static const int VERTEX_COUNT = 3;

    [[nodiscard]] uint32_t min() const {
        return std::min(idxA, std::min(idxB, idxC));
    }
    [[nodiscard]] uint32_t max() const {
        return std::max(idxA, std::max(idxB, idxC));
    }

    void offset(uint32_t offset) {
        idxA += offset;
        idxB += offset;
        idxC += offset;
    }
};

struct MeshData {
    glm::mat4 transform = glm::mat4(1.0f);
    std::vector<Vertex> vertices;
    std::vector<Triangle> indices;
    TextureString texturePath;
    TextureString meshPath;
};

struct TextureData {
    int width;
    int height;
    int bytesPerPixel;
    void* data;
};

enum FileDataType {
    Model_Type,
    Texture_Type
};

#endif//GLMODELVIEWER_MESHDATA_H
"@

$thisPlugin_h = @"
#ifndef ${PluginName}_PLUGIN_H
#define ${PluginName}_PLUGIN_H

#include <Plugin.h>
USE_PLUGIN

extern "C" {
    PLUGIN_API Plugin* PluginInitialise();
};

class ${PluginName}Plugin : public Plugin {
public:
    void Initialise() override;
    void Dispose() override;
    bool SupportsFileType(FileDataType dataType, FilePath& filename) override;
    ModelLoader *GetModelLoader(FilePath& filename) override;
    TextureLoader *GetTextureLoader(FilePath& filename) override;

private:
};

#endif // ${PluginName}_PLUGIN_H

"@

$thisPlugin_cpp = @"
#include <${PluginName}Plugin.h>

void ${PluginName}Plugin::Initialise() {
}

void ${PluginName}Plugin::Dispose() {
}

bool ${PluginName}Plugin::SupportsFileType(FileDataType dataType, FilePath& filename) {
    return false;
}

ModelLoader *${PluginName}Plugin::GetModelLoader(FilePath& filename) {
    return nullptr;
}

TextureLoader *${PluginName}Plugin::GetTextureLoader(FilePath& filename) {
    return nullptr;
}

Plugin *PluginInitialise() {
    return new ${PluginName}Plugin();
}

"@

function Create-Directory($dir)
{
    New-Item -ItemType Directory -Path $dir
}

function Create-File($path, $contents)
{
    New-Item -ItemType File -Path $path -Value $contents
}

Create-Directory $Directory
Push-Location $Directory

Create-Directory "include"
Create-Directory "lib"

Create-File "CMakeLists.txt" $cmakeLists

Add-Content "CMakeLists.txt" @"
if(DEFINED PLUGIN_HEADER_ROOT)
    target_include_directories(${PluginName}Plugin PUBLIC ${dollar}{PLUGIN_HEADER_ROOT})
else()
"@

if ($UseSourceTree)
{
    Add-Content "CMakeLists.txt" "  target_include_directories(${PluginName}Plugin PUBLIC ${SourceTree})"
}
else
{
    Create-Directory "api"
    Add-Content "CMakeLists.txt" "  target_include_directories(${PluginName}Plugin PUBLIC api)"
    Create-File "api/plugin.h" $plugin_h
    Create-File "api/MeshData.h" $meshdata_h
}

Add-Content "CMakeLists.txt" "endif()"

Create-File "include/${PluginName}Plugin.h" $thisPlugin_h
Create-File "lib/${PluginName}Plugin.cpp" $thisPlugin_cpp

if ($UseLocalGLM)
{
    Add-Content -Path "CMakeLists.txt" "target_include_directories(${PluginName}Plugin PUBLIC $glmPath)"
}
else
{
    Create-Directory "external"
    git clone "https://github.com/g-truc/glm" "external/glm"
    Add-Content -Path "CMakeLists.txt" "target_include_directories(${PluginName}Plugin PUBLIC external/glm)"
}

Create-Directory "build"

if($initCmake) {

    Push-Location "build"

    if($useNinja) {
        cmake -GNinja -DSTRING_ENCODING_TYPE=NONE ..
    } else {
        cmake -DSTRING_ENCODING_TYPE=NONE ..
    }

    Pop-Location # "build"
}

Pop-Location # ${Directory}