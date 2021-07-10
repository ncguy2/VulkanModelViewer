//
// Created by Guy on 07/07/2021.
//

#ifndef GLMODELVIEWER_GRIDRENDERER_H
#define GLMODELVIEWER_GRIDRENDERER_H

#include <data/render/AbstractRenderer.h>

class Mesh;

class GridRenderer : public AbstractRenderer {
public:
    void Setup(RendererSetupContext &context) override;
    void Render(RendererContext &context) override;
    const char *Name() override;
    void Resize(int width, int height) override;

    void Dispose() override;

protected:
    std::shared_ptr<ShaderProgram> shader;
    std::shared_ptr<Mesh> mesh;
};

#endif//GLMODELVIEWER_GRIDRENDERER_H
