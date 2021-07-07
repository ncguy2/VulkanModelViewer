//
// Created by Guy on 01/07/2021.
//

#include <plugins/PluginHost.h>
#include <iostream>

PluginHost::PluginHost(const FilePath &path) : path(path) {}

std::shared_ptr<Plugin> PluginHost::Get() {
    if(plugin == nullptr)
        Load();
    return plugin;
}

void PluginHost::Create() {
    if(plugin == nullptr)
        Load();
}

void PluginHost::Load() {
    const wchar_t * c = path.c_str();
    handle = LOAD_PROGRAM_HANDLE(c);
    if(!handle) {
        // TODO implement proper exception handling
        throw std::runtime_error("Unable to load plugin from " + std::string((char*)path.data(), path.size() * 2));
    }

    Initialise = (InitialiseType) LOAD_EXTERN(handle, "PluginInitialise");

    plugin = std::shared_ptr<Plugin>(Initialise());
    plugin->Initialise();
}

void PluginHost::Dispose() {
    if(plugin != nullptr) {
        plugin->Dispose();
        plugin = nullptr;
    }
    Initialise = nullptr;
    FREE_EXTERN(handle);
}

