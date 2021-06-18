#include <iostream>
#include <fstream>
#include <cstring>
#include <thread>
#include <example/ExampleScreen.h>


#include <core/VulkanCore.hpp>
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

std::vector<char> ReadFile(const std::string& filename) {
    std::string p = (CWD + filename) + ".spv";
    std::cout << "Opening file: " << p.c_str() << " for reading" << std::endl;
    std::ifstream file(p, std::ios::ate | std::ios::binary);
    if(!file.is_open())
        throw std::runtime_error("Failed to open file");

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

int shaderIdx = 0;
bool finish = false;

Delegate<VulkanCore*, int, int, int, int>::Signature handle;

int main(int _, char** argv) {
    EXE_PATH = std::string(argv[0]);
    CWD = cwd();
    std::cout << "EXE Path: " << EXE_PATH.c_str() << std::endl;
    std::cout << "CWD: " << CWD.c_str() << std::endl;

    VulkanCore core;
    core.InitVulkan();

    std::shared_ptr<ExampleScreen> screen = std::make_shared<ExampleScreen>();
    core.SetScreen(screen);

    core.MainLoop();

    finish = true;

    core.Cleanup();
    return 0;
}