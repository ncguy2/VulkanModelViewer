#include <iostream>

#include <display/SimpleScreen.h>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    SimpleScreen s;
    s.Loop();

    glfwTerminate();
    return 0;
}