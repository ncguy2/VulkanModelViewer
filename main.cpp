#include <iostream>
#include <cstring>

#include <display/SimpleScreen.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <core/VulkanCore.hpp>
#include <glad/glad.h>
#include <string>

#ifdef WIN32
#include <windows.h>
std::string cwd() {
    char buf[256];
    GetCurrentDirectoryA(256, buf);
    return std::string(buf) + '\\';
}
#endif

std::string EXE_PATH;
std::string CWD;

int main(int _, char** argv) {
    EXE_PATH = std::string(argv[0]);
    CWD = cwd();
    std::cout << "EXE Path: " << EXE_PATH.c_str() << std::endl;
    std::cout << "CWD: " << CWD.c_str() << std::endl;

    VulkanCore core;

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

    /*
     *     vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
     */

    mesh->SetVertices({
            Vertex({  0.0f, -0.5f,  0.0f }, { 1.0f, 0.0f, 0.0f }),
            Vertex({  0.5f,  0.5f,  0.0f }, { 0.0f, 1.0f, 0.0f }),
            Vertex({  -0.5f,  0.5f,  0.0f }, { 0.0f, 0.0f, 1.0f }),

//            {{  0.0f,  0.5f,  0.0f }, { 1.0f, 0.0f, 0.0f }},
//            {{  0.5f, -0.5f,  0.0f }, { 0.0f, 1.0f, 0.0f }},
//            {{  -0.5f, -0.5f,  0.0f }, { 0.0f, 0.0f, 1.0f }}
    });
    core.AddMesh(mesh);

    core.InitVulkan();

    core.MainLoop();

//    SimpleScreen s;
//    s.Loop();

    core.Cleanup();
    return 0;
}