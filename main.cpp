#include <iostream>
#include <fstream>
#include <cstring>
#include <thread>
#include <example/ExampleScreen.h>
#include <plugins/PluginManager.h>
#include <plugins/PluginHost.h>

#include <core/VulkanCore.hpp>
#include <string>

void PlatformStartup();

#ifdef WIN32

#include <windows.h>
#include <io.h>

void PlatformStartup() {
}

std::string cwd() {
    char buf[256];
    GetCurrentDirectoryA(256, buf);
    return std::string(buf) + '\\';
}

#else
void PlatformStartup() {}
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

std::string ConvertWideToNormal(std::wstring wide) {
    std::string n;

    for (auto &item : wide  ) {
        int i = (int) item;
        char a = (char) ((i & 0xFF00) >> 8);
        char b = (char) (i & 0x00FF);

        if(a != '\0')
            n.push_back(a);
        if(b != '\0')
            n.push_back(b);
    }

    return n;
}

int shaderIdx = 0;
bool finish = false;

Delegate<VulkanCore*, int, int, int, int>::Signature handle;

int main(int _, char** argv) {
    PlatformStartup();

//    EXE_PATH = std::string(argv[0]);
//    CWD = cwd();
//    std::cout << "EXE Path: " << EXE_PATH.c_str() << std::endl;
//    std::cout << "CWD: " << CWD.c_str() << std::endl;
//
//    auto pid = _getpid();
//    std::cout << "PID: " << pid << std::endl;
//    getchar();

    VulkanCore core;
    core.InitVulkan();

    std::shared_ptr<ExampleScreen> screen = std::make_shared<ExampleScreen>();
    core.SetScreen(screen);

    core.MainLoop();

    finish = true;

    core.Cleanup();
    return 0;
}