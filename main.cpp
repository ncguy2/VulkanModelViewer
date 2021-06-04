#include <iostream>

#include <display/SimpleScreen.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

bool UsingVulkan = false;

void initGL() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void initVulkan() {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

int main() {
    glfwInit();

//    initGL();
    initVulkan();

    SimpleScreen s;
    s.Loop();

    glfwTerminate();
    return 0;
}