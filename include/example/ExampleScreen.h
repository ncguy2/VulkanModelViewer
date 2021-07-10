//
// Created by Guy on 15/06/2021.
//

#ifndef GLMODELVIEWER_EXAMPLESCREEN_H
#define GLMODELVIEWER_EXAMPLESCREEN_H

#include <core/Screen.h>
#include <core/VulkanCore.hpp>
#include <data/Mesh.h>
#include <data/render/RendererStack.h>
#include <memory>
#include <mutex>
#include <plugins/api/Plugin.h>
#include <queue>

class MeshRenderer;
class GridRenderer;
class BlitRenderer;

class ExampleScreen : public Screen {
public:
    void Update(UpdateContext& context) override;
    void Resize(int width, int height) override;
    void Show() override;
    void Hide() override;
    void Dispose() override;
    void OnKey(int key, int scancode, int action, int mods) override;
    void AddMesh(const std::shared_ptr<Mesh>& mesh);

protected:
    void CleanupMeshes();
    void LoadMesh(MeshData& datum);

    std::vector<std::shared_ptr<Entity>> entities;
    std::shared_ptr<Entity> cameraEntity;
    Camera* cameraPtr;

    // Render
    RendererStack rendererStack;
    std::shared_ptr<RenderPass> offscreenRenderPass;
    std::vector<std::shared_ptr<Texture>> offscreenColourTextures;
    std::vector<std::shared_ptr<Texture>> offscreenDepthTextures;


    std::vector<std::shared_ptr<ShaderProgram>> shaders;
    std::vector<std::shared_ptr<Texture>> textures;
    std::vector<std::shared_ptr<Mesh>> meshes;
    float t;
    float gridScale = 10.0f;
    int width, height;
    int selectedMeshIdx = -1;
    std::shared_ptr<Mesh> GetSelectedMesh();

    VulkanCore::Render::Signature renderFunc;
    VulkanCore::Drop::Signature dropFunc;
    std::mutex meshMutex;
    std::queue<MeshData> meshQueue;
    int meshesToLoad;
    FilePath meshPath;
};

#endif//GLMODELVIEWER_EXAMPLESCREEN_H
