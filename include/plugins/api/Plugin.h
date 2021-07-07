//
// Created by Guy on 01/07/2021.
//

#ifndef GLMODELVIEWER_PLUGIN_H
#define GLMODELVIEWER_PLUGIN_H

#define USE_PLUGIN using namespace Plugins;

#define PLUGIN_BOILERPLATE \
public: \
    void Initialise() override; \
    void Dispose() override; \
    bool SupportsFileType(FileDataType dataType, FilePath filename) override; \
    ModelLoader *GetModelLoader(FilePath filename) override; \
    TextureLoader *GetTextureLoader(FilePath filename) override;

#include <data/MeshData.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <kaitai/kaitaistruct.h>
#include <utility>
#include <codecvt>

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)

#define PLUGIN_API __declspec(dllexport)

#elif defined(__APPLE__) || defined (__linux)

#define PLUGIN_API

#endif

class VulkanCore;

namespace Plugins {

    using FilePath = std::wstring;

//    class FilePath {
//    public:
//        FilePath(std::string filename) : filename(std::move(filename)) {
//            ConvertPathSeparators();
//            BuildComponents();
//        }
//
//        int count() {
//            return pathComponents.size();
//        }
//
//        std::string Component(int idx) {
//            return pathComponents[idx % count()];
//        }
//        std::string ComponentFromEnd(int idx) {
//            idx = (count() - 1) - idx;
//            return Component(idx);
//        }
//
//        std::string Parent() {
//            if(hasParentPathCalculated)
//                return parentPath;
//
//            std::vector<std::string> strings(pathComponents.begin(), pathComponents.end() - 1);
//
//            const char* const delim = "/";
//
//            std::ostringstream imploded;
//            std::copy(strings.begin(), strings.end(),
//                      std::ostream_iterator<std::string>(imploded, delim));
//
//            parentPath = imploded.str();
//            hasParentPathCalculated = true;
//
//            return parentPath;
//        }
//
//        std::string Filename() {
//            return ComponentFromEnd(0);
//        }
//
//        std::string Extension() {
//            auto name = Filename();
//            int idx = name.find(".");
//            if(idx == std::string::npos)
//                return "";
//            return name.substr(idx);
//        }
//
//        bool operator==(const FilePath &rhs) const {
//            return filename == rhs.filename;
//        }
//        bool operator!=(const FilePath &rhs) const {
//            return !(rhs == *this);
//        }
//        bool operator<(const FilePath &rhs) const {
//            return filename < rhs.filename;
//        }
//        bool operator>(const FilePath &rhs) const {
//            return rhs < *this;
//        }
//        bool operator<=(const FilePath &rhs) const {
//            return !(rhs < *this);
//        }
//        bool operator>=(const FilePath &rhs) const {
//            return !(*this < rhs);
//        }
//
//        operator const char*() { // NOLINT(google-explicit-constructor)
//            return filename.c_str();
//        }
//
//        std::string string() {
//            return filename;
//        }
//        const char* cstring() {
//            return filename.c_str();
//        }
//
//        bool HasWideComponents() {
//            for(int i = filename.length() - 1; i >= 0; i--) {
//                if(filename[i] == '\0')
//                    return true;
//            }
//
//            return false;
//        }
//
//        bool IsWide(std::string c) {
//            for(int i = c.length() - 1; i >= 0; i--) {
//                if(c[i] == '\0')
//                    return true;
//            }
//
//            return false;
//        }
//
//        std::wstring WidenComponent(int idx) {
//            std::string s = Component(idx);
//            if(IsWide(s)) {
//                return std::wstring((wchar_t*) s.data(), s.size()/2);
//            }
//
//            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
//            return converter.from_bytes(s);
//
////            int len;
////            int slength = (int)s.length() + 1;
////            len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
////            wchar_t* buf = new wchar_t[len];
////            MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
////            std::wstring r(buf);
////            delete[] buf;
////            return r;
//        }
//
//        std::wstring Widen() {
//            std::wstring s;
//
//            for(int i = 0; i < pathComponents.size(); i++) {
//                s += WidenComponent(i);
//                if(i != pathComponents.size() - 1)
//                    s += L"/";
//            }
//
//            return s;
//        }
//
//    protected:
//        void ConvertPathSeparators() {
//            std::replace(filename.begin(), filename.end(), '\\', '/');
//        }
//        void BuildComponents() {
//            std::string delimiter = "/";
//            size_t last = 0;
//            size_t next = 0;
//            while((next = filename.find(delimiter, last)) != std::string::npos) {
//                pathComponents.push_back(filename.substr(last, next - last));
//                last = next + 1;
//            }
//            pathComponents.push_back(filename.substr(last));
//        }
//
//        std::string filename;
//        bool hasParentPathCalculated = false;
//        std::string parentPath;
//        std::vector<std::string> pathComponents;
//    };


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

#define KAITAI_INIT(Type, variable) Type *variable = nullptr

#define KAITAI_READ(Type, variable, path)               \
    {                                                   \
        std::ifstream ifs(path, std::ifstream::binary); \
        kaitai::kstream ks(&ifs);                       \
        variable = new Type(&ks);                       \
    }

#define KAITAI_INIT_READ(Type, variable, path) KAITAI_INIT(Type, variable); KAITAI_READ(Type, variable, path);

};// namespace Plugins

#endif//GLMODELVIEWER_PLUGIN_H
