//
// Created by Guy on 09/07/2021.
//

#ifndef GLMODELVIEWER_STARTUP_H
#define GLMODELVIEWER_STARTUP_H

#define WIN32_LEAN_AND_MEAN
#include <GLFW/glfw3.h>
#include <windows.h>

void Start(HWND hwnd);

extern "C" {
    // Control
    __declspec(dllexport) void SetAssetRoot(const char* path);
    __declspec(dllexport) void StartExtern(HWND hwnd);
    __declspec(dllexport) void Shutdown();
    __declspec(dllexport) void Log(const char* msg);

    // Inputs
    __declspec(dllexport) void Key(int keycode, int action);
    __declspec(dllexport) void Drop(int pathCount, const char** paths);

    // Outputs
    __declspec(dllexport) void SetModelInfoFunc(void* funcPtr);
}

#endif//GLMODELVIEWER_STARTUP_H
