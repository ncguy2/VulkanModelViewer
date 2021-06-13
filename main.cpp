#include <iostream>
#include <fstream>
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

int main(int _, char** argv) {
    EXE_PATH = std::string(argv[0]);
    CWD = cwd();
    std::cout << "EXE Path: " << EXE_PATH.c_str() << std::endl;
    std::cout << "CWD: " << CWD.c_str() << std::endl;

    VulkanCore core;
    core.InitVulkan();

    std::vector<char> vertShaderCode = ReadFile("assets/shaders/sample/sample.vert");
    std::vector<char> fragShaderCode = ReadFile("assets/shaders/sample/sample.frag");

    std::shared_ptr<ShaderProgram> shader = core.CreateShaderProgram();
    shader->AddStage(ShaderStage("main", vk::ShaderStageFlagBits::eVertex, vertShaderCode));
    shader->AddStage(ShaderStage("main", vk::ShaderStageFlagBits::eFragment, fragShaderCode));
    shader->AddSamplers(2);

    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

    mesh->SetVertices({
            Vertex({  -0.5f, -0.5f,   0.0f }, { 1.0f, 0.0f, 0.0f }, {1.0f, 0.0f}),
            Vertex({   0.5f, -0.5f,   0.0f }, { 0.0f, 1.0f, 0.0f }, {0.0f, 0.0f}),
            Vertex({   0.5f,  0.5f,   0.0f }, { 0.0f, 0.0f, 1.0f }, {0.0f, 1.0f}),
            Vertex({  -0.5f,  0.5f,   0.0f }, { 1.0f, 1.0f, 1.0f }, {1.0f, 1.0f}),

            Vertex({  -0.5f, -0.5f,  -0.5f }, { 1.0f, 0.0f, 0.0f }, {1.0f, 0.0f}),
            Vertex({   0.5f, -0.5f,  -0.5f }, { 0.0f, 1.0f, 0.0f }, {0.0f, 0.0f}),
            Vertex({   0.5f,  0.5f,  -0.5f }, { 0.0f, 0.0f, 1.0f }, {0.0f, 1.0f}),
            Vertex({  -0.5f,  0.5f,  -0.5f }, { 1.0f, 1.0f, 1.0f }, {1.0f, 1.0f}),
    });
    mesh->SetIndices({
            Triangle(0, 1, 2),
            Triangle(2, 3, 0),

            Triangle(4, 5, 6),
            Triangle(6, 7, 4),
    });

    mesh->SetShaderProgram(shader);
    std::shared_ptr<Texture> texture = core.CreateTexture(vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
    texture->LoadFromFile(R"(F:\Linked\Downloads\texture.jpg)");
    shader->SetTexture(texture->GetView(), 0);
    std::shared_ptr<Texture> face = core.CreateTexture(vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
    face->LoadFromFile(R"(F:\Linked\Downloads\awesomeface.png)");
    shader->SetTexture(face->GetView(), 1);
    core.CompileShader(shader);

    core.AddMesh(mesh);

    core.MainLoop();

//    SimpleScreen s;
//    s.Loop();

    texture.reset();
    shader->Cleanup();
    core.Cleanup();
    return 0;
}