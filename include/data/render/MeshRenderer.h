//
// Created by Guy on 20/06/2021.
//

#ifndef GLMODELVIEWER_MESHRENDERER_H
#define GLMODELVIEWER_MESHRENDERER_H

#include "AbstractRenderer.h"

class MeshRenderer : public AbstractRenderer {
public:
    void Render(RendererContext &context) override;
};

#endif//GLMODELVIEWER_MESHRENDERER_H
