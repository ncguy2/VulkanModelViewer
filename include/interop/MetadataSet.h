//
// Created by Guy on 17/07/2021.
//

#ifndef GLMODELVIEWER_METADATASET_H
#define GLMODELVIEWER_METADATASET_H

#include <interop/MetadataRecord.h>
#include <vector>
#include <any>
#include <memory>


class MetadataSet {
public:
    template <typename T>
    void AddRecord(Record<T> record) {
        auto ptr = std::make_shared<Record<T>>(record);
        data.records.push_back(ptr);
    }

    void ClearRecords();

    void* Data();

protected:
    struct _data {
        int count;
        std::vector<std::any> records;
    } data;


};

#endif//GLMODELVIEWER_METADATASET_H
