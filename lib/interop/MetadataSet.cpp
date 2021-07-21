//
// Created by Guy on 17/07/2021.
//

#include <interop/MetadataSet.h>

void MetadataSet::ClearRecords() {
    data.records.clear();
}

void *MetadataSet::Data() {
    data.count = data.records.size();
    return &data;
}
