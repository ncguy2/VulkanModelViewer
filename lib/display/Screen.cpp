//
// Created by Guy on 03/06/2021.
//

#include <display/Screen.h>
#include <iostream>
#include <string>
#include <GLFW/glfw3.h>

#define S_CAST(win) static_cast<Screen*>(glfwGetWindowUserPointer(win))

char const* gl_error_string(GLenum const err) noexcept;

bool Screen::Loop() {
    window = glfwCreateWindow(800, 600, "asdf", nullptr, nullptr);
    if(window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* win, int w, int h) { S_CAST(win)->Resize(w, h); });
    glfwSetKeyCallback(window, [](GLFWwindow* win, int key, int scancode, int action, int mods) { S_CAST(win)->OnKey(key, scancode, action, mods); });

    Setup();

    while(!glfwWindowShouldClose(window)) {
        ProcessInputs();

        Render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        CheckErrors();
    }

    return true;
}

void Screen::ProcessInputs() {

}

void Screen::OnKey(int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Screen::Resize(int width, int height) {
    glViewport(0, 0, width, height);
}

void Screen::CheckErrors() {
    unsigned int error = glGetError();
    while(error != GL_NO_ERROR) {
        std::cout << "Error: " << gl_error_string(error) << std::endl;
        error = glGetError();
    }
}

char const* gl_error_string(GLenum const err) noexcept {
    switch (err) {
        // opengl 2 errors (8)
        case GL_NO_ERROR:
            return "GL_NO_ERROR";

        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";

        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";

        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";

        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";

            // opengl 3 errors (1)
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";

            // gles 2, 3 and gl 4 error are handled by the switch above
        default:
            return std::to_string(err).c_str();
    }
}