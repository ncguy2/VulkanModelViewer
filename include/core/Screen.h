//
// Created by Guy on 15/06/2021.
//

#ifndef GLMODELVIEWER_SCREEN_H
#define GLMODELVIEWER_SCREEN_H

#include <ecs/Scene.h>
#include <vulkan/vulkan.hpp>

class VulkanCore;
class Mesh;
class PluginManager;

struct CoreScreenComponents {
    VulkanCore* core;
    vk::Device* device;
    vk::CommandPool* commandPool;
    PluginManager* pluginManager;
};

class Screen {
public:

    void AssignCoreComponents(CoreScreenComponents& c);
    bool IsCreated() const;

    virtual void Create();
    virtual void Update(UpdateContext& context) = 0;
    virtual void Resize(int width, int height) = 0;
    virtual void Show() = 0;
    virtual void Hide() = 0;
    virtual void Dispose() = 0;
    virtual void OnKey(int key, int scancode, int action, int mods) = 0;

    EntityScene* GetScenePtr();

protected:
    EntityScene scene;
    CoreScreenComponents components;
    bool isCreated = false;
};

class ScreenAdapter : public Screen {
public:
    void Create() override;
    void Update(UpdateContext& context) override;
    void Resize(int width, int height) override;
    void Show() override;
    void Hide() override;
    void Dispose() override;
    void OnKey(int key, int scancode, int action, int mods) override;
};


class ScreenUtils {
public:
    static void RecordMeshes(int idx, vk::CommandBuffer& buffer, std::vector<std::shared_ptr<Mesh>>& meshes, CoreScreenComponents& components);
};

#endif//GLMODELVIEWER_SCREEN_H
