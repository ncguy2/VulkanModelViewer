//
// Created by Guy on 20/06/2021.
//

#include <data/render/AbstractRenderer.h>

void AbstractRenderer::Setup(RendererSetupContext& context) {
    setupContext = context;
}

void AbstractRenderer::Dispose() {}

void AbstractRenderer::Resize(int width, int height) {}
