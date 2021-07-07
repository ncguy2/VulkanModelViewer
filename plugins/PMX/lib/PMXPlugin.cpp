//
// Created by Guy on 01/07/2021.
//

#include <PMXPlugin.h>
#include <loader/PMXLoader.h>

void PMXPlugin::Dispose() {
    pmxLoader.reset();
}

void PMXPlugin::Initialise() {
    pmxLoader = std::make_shared<PMXLoader>(this);
}

bool PMXPlugin::SupportsFileType(FileDataType dataType, FilePath& filename) {
    return filename.ends_with(L".pmx");
}

ModelLoader *PMXPlugin::GetModelLoader(FilePath& filename) {
    if(filename.ends_with(L".pmx"))
        return reinterpret_cast<ModelLoader*>(pmxLoader.get());
    return nullptr;
}

TextureLoader *PMXPlugin::GetTextureLoader(FilePath& filename) {
    return nullptr;
}


Plugin *PluginInitialise() {
    return new PMXPlugin();
}
