//
// Created by Guy on 20/06/2021.
//

#ifndef GLMODELVIEWER_BLITRENDERER_H
#define GLMODELVIEWER_BLITRENDERER_H

#include <data/render/AbstractRenderer.h>

class BlitRenderer : public AbstractRenderer {
public:
    void Setup(RendererSetupContext &context) override;
    void SetTexture(int slot, Texture* texPtr);
    void Render(RendererContext &context) override;

protected:
    std::shared_ptr<ShaderProgram> shader;
    std::shared_ptr<Mesh> mesh;
};

#endif//GLMODELVIEWER_BLITRENDERER_H
