//
// Created by Guy on 03/06/2021.
//

#include <pch.h>
#include <display/SimpleScreen.h>

//VERTEX_ATTRIBUTES()

void SimpleScreen::Setup() {
    const char* vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "layout (location = 1) in vec3 aColour;\n"
                                     "out vec3 Colour;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   Colour = aColour;\n"
                                     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColour;\n"
                                       "in vec3 Colour;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColour = vec4(Colour, 1.0f);\n"
                                       "}\0";

//    shader.SetVertexSource(vertexShaderSource);
//    shader.SetFragmentSource(fragmentShaderSource);
    shader.Compile();

    std::vector<float> v = {
             0.5f,  0.5f, 0.0f,  1.0f,  1.0f, 1.0f,
             0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 0.0f,
            -0.5f, -0.5f, 0.0f,  0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f, 0.0f,  0.0f,  0.0f, 1.0f,
    };
    std::vector<unsigned int> i = {
            0, 1, 3,
            1, 2, 3
    };
//    mesh.SetVertices(v);
//    mesh.SetIndices(i);
//    mesh.AddAttribute(VertexAttributes::Vertex);
//    mesh.AddAttribute(VertexAttributes::Colour);
//    mesh.Initialize();
}

void SimpleScreen::Render() {
    glClearColor(.2f, .3f, .3f, 1);
    glClear(GL_COLOR_BUFFER_BIT);

//    shader.Bind();
//    mesh.Draw();

}
