//
// Created by Guy on 21/07/2021.
//

#ifndef GLMODELVIEWER_OBJLOADER_H
#define GLMODELVIEWER_OBJLOADER_H

#include <Plugin.h>
USE_PLUGIN

class ObjLoader : public ModelLoader {
public:
    ObjLoader(Plugin* plugin);
    std::vector<MeshData> Load(FilePath& filePath) override;
};

#endif//GLMODELVIEWER_OBJLOADER_H
