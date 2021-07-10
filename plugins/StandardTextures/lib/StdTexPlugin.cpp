//
// Created by Guy on 03/07/2021.
//

#include <StdTexPlugin.h>
#include <StdTexLoader.h>

void StdTexPlugin::Initialise() {
    texLoader = std::make_shared<StdTexLoader>(this);
}

void StdTexPlugin::Dispose() {
    texLoader.reset();
}

bool StdTexPlugin::SupportsFileType(FileDataType dataType, FilePath& filename) {
    if(dataType != Texture_Type)
        return false;

    bool supports = filename.ends_with(L".png") ||
                    filename.ends_with(L".jpg") ||
                    filename.ends_with(L".bmp");
    return supports;
}

ModelLoader *StdTexPlugin::GetModelLoader(FilePath& filename) {
    return nullptr;
}

TextureLoader *StdTexPlugin::GetTextureLoader(FilePath& filename) {
    return reinterpret_cast<TextureLoader *>(texLoader.get());
}

Plugin *PluginInitialise() {
    return new StdTexPlugin();
}
