#ifndef Obj_PLUGIN_H
#define Obj_PLUGIN_H

#include "ObjLoader.h"
#include <Plugin.h>
USE_PLUGIN

extern "C" {
    PLUGIN_API Plugin* PluginInitialise();
};

class ObjPlugin : public Plugin {
public:
    void Initialise() override;
    void Dispose() override;
    bool SupportsFileType(FileDataType dataType, FilePath& filename) override;
    ModelLoader *GetModelLoader(FilePath& filename) override;
    TextureLoader *GetTextureLoader(FilePath& filename) override;

private:
    std::shared_ptr<ObjLoader> loader;
};

#endif // Obj_PLUGIN_H
