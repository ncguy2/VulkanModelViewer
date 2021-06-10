//
// Created by Guy on 03/06/2021.
//

#ifndef GLMODELVIEWER_SCREEN_H
#define GLMODELVIEWER_SCREEN_H

#include <glad/glad.h>

class GLFWwindow;

class Screen {
public:
    virtual ~Screen();

    bool Loop();
    virtual void Resize(int width, int height);
    virtual void OnKey(int key, int scancode, int action, int mods);
    virtual void ProcessInputs();

    virtual void Setup() = 0;
    virtual void Render() = 0;

private:
    GLFWwindow* window;

    void CheckErrors();
};

#endif//GLMODELVIEWER_SCREEN_H
