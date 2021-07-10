//
// Created by Guy on 07/07/2021.
//

#ifndef GLMODELVIEWER_RENDERERSET_H
#define GLMODELVIEWER_RENDERERSET_H

#include <vulkan/vulkan.hpp>
#include <vector>
#include <data/Contexts.h>

class AbstractRenderer;

class RendererStack {
public:
    void Setup(RendererSetupContext& context);
    void Render(RendererContext& context);
    void Dispose();

    template <typename T>
    void AddRenderer() {
        auto ptr = std::make_shared<T>();
        stack.push_back(ptr);
    }

    bool IsEnabled(int slot);
    void Toggle(int slot);
    void Resize(int width, int height);

protected:
    RendererSetupContext setupContext;
    std::vector<vk::CommandBuffer> commandBuffers;
    std::vector<std::shared_ptr<AbstractRenderer>> stack;
    int disabledMask = 0;
};

#endif//GLMODELVIEWER_RENDERERSET_H
