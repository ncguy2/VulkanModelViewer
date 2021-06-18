//
// Created by Guy on 13/06/2021.
//

#ifndef GLMODELVIEWER_RENDERPIPELINE_H
#define GLMODELVIEWER_RENDERPIPELINE_H

#include <data/Shader.h>

class RenderPipeline {
public:

    void Construct();

protected:
    std::vector<vk::ImageView> imageViews;
    std::vector<std::shared_ptr<ShaderProgram>> shaders;
};

#endif//GLMODELVIEWER_RENDERPIPELINE_H
