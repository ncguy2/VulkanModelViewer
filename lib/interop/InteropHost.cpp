//
// Created by Guy on 17/07/2021.
//

#include <interop/InteropHost.h>

FUNC_PTR_DEF(Signal);
FUNC_PTR_DEF(ModelInfo);
FUNC_PTR_DEF(MetadataSource);
FUNC_PTR_DEF(MetadataClear);

void InteropHost::Call_Signal(unsigned int flags) {
    FUNC_PTR_CHECK(Signal);
    SignalFuncPtr(flags);
}

void InteropHost::Call_ModelInfo(int size, int slot, const char * string) {
    FUNC_PTR_CHECK(ModelInfo);
    ModelInfoFuncPtr(size, slot, string);
}

void InteropHost::Call_MetadataSource(void * ptr) {
    FUNC_PTR_CHECK(MetadataSource);
    MetadataSourceFuncPtr(ptr);
}

void InteropHost::Call_MetadataClear() {
    FUNC_PTR_CHECK(MetadataSource);
    MetadataClearFuncPtr();
}
