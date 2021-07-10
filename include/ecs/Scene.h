//
// Created by Guy on 16/06/2021.
//

#ifndef GLMODELVIEWER_SCENE_H
#define GLMODELVIEWER_SCENE_H

#include <vulkan/vulkan.hpp>
#include <data/Contexts.h>
#include <ecs/Entity.h>
#include <memory>
#include <vector>


class VulkanCore;
class Component;

class EntityScene {
public:

    std::shared_ptr<Entity> CreateEntity();
    void Update(float delta, UpdateContext &context);

    template<typename T, typename =
    std::enable_if_t<std::is_base_of_v<Component, std::remove_reference_t<T>>>>
    void RemoveEntitiesWith() {
//        std::erase(std::remove_if(entities.begin(), entities.end(),
//          [](const std::shared_ptr<Entity>& ePtr) {
//            return ePtr->HasComponent<T>();
//          }), entities.end());

        auto it = entities.begin();
        while (it != entities.end()) {
            if((*it)->HasComponent<T>())
                it = entities.erase(it);
            else ++it;
        }
    }

    void RemoveEntities();

protected:
    std::vector<std::shared_ptr<Entity>> entities;
};

#endif//GLMODELVIEWER_SCENE_H
