//
// Created by Guy on 01/07/2021.
//

#ifndef GLMODELVIEWER_PMXLOADER_H
#define GLMODELVIEWER_PMXLOADER_H

#include <Plugin.h>
#include <core/Events.h>
USE_PLUGIN

class PMXLoader : public ModelLoader {
public:
    PMXLoader(Plugin *plugin);
    std::vector<MeshData> Load(FilePath& filename) override;
    void LoadAsync(FilePath& filename, Delegate<MeshData&>::Signature callback) override;
};

#endif//GLMODELVIEWER_PMXLOADER_H
