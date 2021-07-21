//
// Created by Guy on 17/07/2021.
//

#ifndef GLMODELVIEWER_INTEROPHOST_H
#define GLMODELVIEWER_INTEROPHOST_H

#define FUNC_PTR_NAME(name) Func_##name##_type
#define FUNC_PTR(name, ...) \
protected: \
    void*  name##_ptr = nullptr; \
    typedef void* (*FUNC_PTR_NAME(name))(__VA_ARGS__); \
    FUNC_PTR_NAME(name) name##FuncPtr = nullptr;                          \
public: \
    void Call_##name(__VA_ARGS__);                     \
    void Set_##name(void* ptr);

#define FUNC_PTR_DEF(name) \
    void InteropHost::Set_##name(void* ptr) {                     \
        name##_ptr = ptr; \
        name##FuncPtr = reinterpret_cast<FUNC_PTR_NAME(name)>(ptr);                    \
    }

#define FUNC_PTR_CHECK(name) \
    if(!name##FuncPtr) \
        return

#define FUNC_REGISTER(name) \
    case name: \
        interop.Set_##name(ptr); \
        return true

class InteropHost {
protected:
    FUNC_PTR(Signal, unsigned int);
    FUNC_PTR(ModelInfo, int, int, const char*);
    FUNC_PTR(MetadataSource, void*);
    FUNC_PTR(MetadataClear);
};

#endif//GLMODELVIEWER_INTEROPHOST_H
