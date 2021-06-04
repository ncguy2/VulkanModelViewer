//
// Created by Guy on 03/06/2021.
//

#ifndef GLMODELVIEWER_SHADER_H
#define GLMODELVIEWER_SHADER_H

#include <string>

class ShaderProgram {
public:
    virtual ~ShaderProgram();

    void SetVertexSource(const char* source);
    void SetFragmentSource(const char* source);

    void Compile();
    void Bind();
protected:
    unsigned int shaderProgram;
    const char* vertexSource;
    const char* fragmentSource;
};

#endif//GLMODELVIEWER_SHADER_H
