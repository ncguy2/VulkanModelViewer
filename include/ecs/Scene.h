//
// Created by Guy on 16/06/2021.
//

#ifndef GLMODELVIEWER_SCENE_H
#define GLMODELVIEWER_SCENE_H

#include <vulkan/vulkan.hpp>

#include <memory>
#include <vector>

class Entity;
class VulkanCore;

class EntityScene {
public:

    std::shared_ptr<Entity> CreateEntity();
    void Update(float delta);
    void Record(uint32_t idx, vk::CommandBuffer& buffer, VulkanCore* core);

protected:
    std::vector<std::shared_ptr<Entity>> entities;
};

#endif//GLMODELVIEWER_SCENE_H
