//
// Created by Guy on 01/07/2021.
//

#ifndef GLMODELVIEWER_PMXPLUGIN_H
#define GLMODELVIEWER_PMXPLUGIN_H

#include <memory>
#include <Plugin.h>
USE_PLUGIN

extern "C" {
    PLUGIN_API Plugin* PluginInitialise();
};

class PMXLoader;

class PMXPlugin : public Plugins::Plugin {
public:
    void Initialise() override;
    void Dispose() override;
    bool SupportsFileType(FileDataType dataType, FilePath& filename) override;

    ModelLoader *GetModelLoader(FilePath& filename) override;
    TextureLoader *GetTextureLoader(FilePath& filename) override;

private:
    std::shared_ptr<PMXLoader> pmxLoader;
};

#endif//GLMODELVIEWER_PMXPLUGIN_H
