//
// Created by Guy on 01/07/2021.
//

#ifndef GLMODELVIEWER_PLUGINHOST_H
#define GLMODELVIEWER_PLUGINHOST_H

#include "api/Plugin.h"
using namespace Plugins;

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#include <windows.h>
typedef HMODULE Handle;

#define LOAD_PROGRAM_HANDLE(filename) LoadLibraryW(filename)
#define LOAD_EXTERN GetProcAddress
#define FREE_EXTERN FreeLibrary
#define LIBRARY_SUFFIX ".dll"

#elif defined(__APPLE__) || defined (__linux)
#error "Non-windows platforms not yet supported"

#include <dlfcn.h>

// TODO Populate the non-windows macros here
typedef void* Handle;
#define LOAD_PROGRAM_HANDLE(filename) dlopen(filename, RTLD_LAZY)
#define LOAD_EXTERN dlsym
#define FREE_EXTERN dlclose
#define LIBRARY_SUFFIX ".so"

#endif

class PluginManager;

class PluginHost {
public:
    PluginHost(const FilePath &path);

    /**
     * Returns a pointer to the plugin, resolving and loading it if necessary.
     * @return
     */
    std::shared_ptr<Plugin> Get();
    void Create();
    void Dispose();

protected:
    friend PluginManager;

    void Load();

    typedef Plugin*(*InitialiseType)();
    InitialiseType Initialise;
//    Plugin*(*Initialise)();

    FilePath path;
    Handle handle = nullptr;
    std::shared_ptr<Plugin> plugin = nullptr;
};


#endif//GLMODELVIEWER_PLUGINHOST_H
