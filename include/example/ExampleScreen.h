//
// Created by Guy on 15/06/2021.
//

#ifndef GLMODELVIEWER_EXAMPLESCREEN_H
#define GLMODELVIEWER_EXAMPLESCREEN_H

#include <core/Screen.h>
#include <core/VulkanCore.hpp>
#include <data/Mesh.h>
#include <memory>
#include <plugins/api/Plugin.h>

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
    void CleanupMeshes();
    void LoadMeshes(Plugins::FilePath path);

    std::vector<std::shared_ptr<Entity>> entities;
    std::shared_ptr<Entity> camera;

    // Render
    std::shared_ptr<MeshRenderer> meshRenderer;
    std::shared_ptr<RenderPass> offscreenRenderPass;
    std::vector<std::shared_ptr<Texture>> offscreenColourTextures;
    std::vector<std::shared_ptr<Texture>> offscreenDepthTextures;


    std::vector<std::shared_ptr<ShaderProgram>> shaders;
    std::vector<std::shared_ptr<Texture>> textures;
    std::vector<std::shared_ptr<Mesh>> meshes;
    float t;
    int width, height;
    int selectedMeshIdx = -1;
    std::shared_ptr<Mesh> GetSelectedMesh();

    VulkanCore::Render::Signature renderFunc;
    VulkanCore::Drop::Signature dropFunc;
};

#endif//GLMODELVIEWER_EXAMPLESCREEN_H
