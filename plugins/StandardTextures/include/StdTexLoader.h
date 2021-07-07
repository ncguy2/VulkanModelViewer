//
// Created by Guy on 03/07/2021.
//

#ifndef GLMODELVIEWER_STDTEXLOADER_H
#define GLMODELVIEWER_STDTEXLOADER_H

#include <Plugin.h>
#include <map>

using namespace Plugins;

class StdTexLoader : public TextureLoader {
public:
    StdTexLoader(Plugin *plugin);
    virtual ~StdTexLoader();
    TextureData Load(FilePath& filename) override;
    void Free(TextureData& texData) override;

private:
    std::map<FilePath, TextureData> cache{};
};

#endif//GLMODELVIEWER_STDTEXLOADER_H
