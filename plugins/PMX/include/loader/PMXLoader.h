//
// Created by Guy on 01/07/2021.
//

#ifndef GLMODELVIEWER_PMXLOADER_H
#define GLMODELVIEWER_PMXLOADER_H

#include <Plugin.h>
USE_PLUGIN

class PMXLoader : public ModelLoader {
public:
    PMXLoader(Plugin *plugin);
    std::vector<MeshData> Load(FilePath& filename) override;
};

#endif//GLMODELVIEWER_PMXLOADER_H
