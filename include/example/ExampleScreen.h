//
// Created by Guy on 15/06/2021.
//

#ifndef GLMODELVIEWER_EXAMPLESCREEN_H
#define GLMODELVIEWER_EXAMPLESCREEN_H

#include <core/VulkanCore.hpp>
#include <core/Screen.h>
#include <data/Mesh.h>
#include <memory>

class MeshRenderer;
class BlitRenderer;

class ExampleScreen : public Screen {
public:
    void Update(float delta) override;
    void Resize(int width, int height) override;
    void Show() override;
    void Hide() override;
    void Dispose() override;
    void OnKey(int key, int scancode, int action, int mods) override;

    void AddMesh(const std::shared_ptr<Mesh>& mesh);

protected:
    std::shared_ptr<Entity> entity;
    std::shared_ptr<Entity> camera;

    // Render
    std::shared_ptr<MeshRenderer> meshRenderer;
    std::shared_ptr<BlitRenderer> blitRenderer;
    std::shared_ptr<RenderPass> offscreenRenderPass;
    std::vector<std::shared_ptr<Texture>> offscreenColourTextures;
    std::vector<std::shared_ptr<Texture>> offscreenDepthTextures;


    std::shared_ptr<ShaderProgram> shader;
    std::shared_ptr<Texture> texture;
    std::vector<std::shared_ptr<Mesh>> meshes;
    float t;
    int width, height;

    VulkanCore::Render::Signature renderFunc;
};

#endif//GLMODELVIEWER_EXAMPLESCREEN_H
