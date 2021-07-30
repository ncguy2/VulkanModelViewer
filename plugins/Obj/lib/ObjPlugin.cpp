#include <ObjPlugin.h>

void ObjPlugin::Initialise() {
    loader = std::make_shared<ObjLoader>(this);
}

void ObjPlugin::Dispose() {
    loader = nullptr;
}

bool ObjPlugin::SupportsFileType(FileDataType dataType, FilePath& filename) {
    return dataType == Model_Type && filename.ends_with(L".obj");
}

ModelLoader *ObjPlugin::GetModelLoader(FilePath& filename) {
    return loader.get();
}

TextureLoader *ObjPlugin::GetTextureLoader(FilePath& filename) {
    return nullptr;
}

Plugin *PluginInitialise() {
    return new ObjPlugin();
}
