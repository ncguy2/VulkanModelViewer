//
// Created by Guy on 03/07/2021.
//

#ifndef GLMODELVIEWER_STDTEXPLUGIN_H
#define GLMODELVIEWER_STDTEXPLUGIN_H

#include <Plugin.h>
USE_PLUGIN

extern "C" {
    PLUGIN_API Plugin* PluginInitialise();
};

class StdTexLoader;

class StdTexPlugin : public Plugin {
public:
    void Initialise() override;
    void Dispose() override;
    bool SupportsFileType(FileDataType dataType, FilePath& filename) override;
    ModelLoader *GetModelLoader(FilePath& filename) override;
    TextureLoader *GetTextureLoader(FilePath& filename) override;

private:
    std::shared_ptr<StdTexLoader> texLoader;
};

#endif//GLMODELVIEWER_STDTEXPLUGIN_H
