//
// Created by Guy on 09/07/2021.
//

#ifndef GLMODELVIEWER_STARTUP_H
#define GLMODELVIEWER_STARTUP_H

#define WIN32_LEAN_AND_MEAN
#include <GLFW/glfw3.h>
#include <windows.h>

void Start(HWND hwnd);

enum FunctionIds : uint8_t {
    Signal = 0,
    ModelInfo = 1,
    MetadataSource = 2,
    MetadataClear = 3
};

extern "C" {
    // Control
    __declspec(dllexport) void SetAssetRoot(const char* path);
    __declspec(dllexport) void StartExtern(HWND hwnd);
    __declspec(dllexport) void Shutdown();
    __declspec(dllexport) void Log(const char* msg);

    // Inputs
    __declspec(dllexport) void Key(int keycode, int action);
    __declspec(dllexport) void Drop(int pathCount, const char** paths);

    __declspec(dllexport) void SetRegistryValue_ulong(const char* path, uint64_t value);
    __declspec(dllexport) void SetRegistryValue_int(const char* path, int value);
    __declspec(dllexport) void SetRegistryValue_bool(const char* path, bool value);
    __declspec(dllexport) void SetRegistryValue_float(const char* path, float value);
    __declspec(dllexport) void SetRegistryValue_string(const char* path, const char* value);

    // Outputs
    __declspec(dllexport) uint64_t GetRegistryValue_ulong(const char* path);
    __declspec(dllexport) unsigned int FPS(bool smoothed);

    // Functions
    __declspec(dllexport) bool SetFunctionPointer(FunctionIds function, void* ptr);
}

#endif//GLMODELVIEWER_STARTUP_H
