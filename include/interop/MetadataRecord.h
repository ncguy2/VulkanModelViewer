//
// Created by Guy on 17/07/2021.
//

#ifndef GLMODELVIEWER_METADATARECORD_H
#define GLMODELVIEWER_METADATARECORD_H

enum RecordType {
    TestRecord = 0x0,
    // Flags
    ModelDataFlag = 0x0001,
    SceneDataFlag = 0x0002,
    MaterialDataFlag = 0x0004,
    ShaderDataFlag = 0x0008,

    // Concrete
    ModelData = (0x0001 << 8) | ModelDataFlag,
};

template<typename T>
struct Record {
    RecordType type;
    int size;
    T data;
};

namespace Records {
    struct MeshData {
        int meshCount;
        int textureCount;
        int meshCreationTime;
//        const char *meshPath;
    };
}

#endif//GLMODELVIEWER_METADATARECORD_H
