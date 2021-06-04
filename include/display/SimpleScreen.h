//
// Created by Guy on 03/06/2021.
//

#ifndef GLMODELVIEWER_SIMPLESCREEN_H
#define GLMODELVIEWER_SIMPLESCREEN_H

#include "Screen.h"
#include <data/Mesh.h>
#include <data/Shader.h>

class SimpleScreen : public Screen {
public:
    void Setup() override;
    void Render() override;

private:
    ShaderProgram shader;
    Mesh mesh;
};

#endif//GLMODELVIEWER_SIMPLESCREEN_H
