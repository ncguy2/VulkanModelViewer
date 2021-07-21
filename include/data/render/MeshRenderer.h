//
// Created by Guy on 20/06/2021.
//

#ifndef GLMODELVIEWER_MESHRENDERER_H
#define GLMODELVIEWER_MESHRENDERER_H

#include "AbstractRenderer.h"

class Camera;

class MeshRenderer : public AbstractRenderer {
public:
    void Setup(RendererSetupContext &context) override;
    void Render(RendererContext &context) override;
    const char *Name() override;
    void Dispose() override;
    void Resize(int width, int height) override;

    Camera* camera = nullptr;

protected:
    std::shared_ptr<ShaderProgram> geometryShader = nullptr;
};

#endif//GLMODELVIEWER_MESHRENDERER_H
