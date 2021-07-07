//
// Created by Guy on 16/06/2021.
//

#ifndef GLMODELVIEWER_SCENE_H
#define GLMODELVIEWER_SCENE_H

#include <vulkan/vulkan.hpp>
#include <data/Contexts.h>
#include <memory>
#include <vector>

class Entity;
class VulkanCore;
class Component;

class EntityScene {
public:

    std::shared_ptr<Entity> CreateEntity();
    void Update(float delta, UpdateContext &context);

    template<typename T, typename =
    std::enable_if_t<std::is_base_of_v<Component, std::remove_reference_t<T>>>>
    void RemoveEntitiesWith() {
        // TODO implement this
    }

    void RemoveEntities();

protected:
    std::vector<std::shared_ptr<Entity>> entities;
};

#endif//GLMODELVIEWER_SCENE_H
