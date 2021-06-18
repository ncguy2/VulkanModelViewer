//
// Created by Guy on 16/06/2021.
//

#ifndef GLMODELVIEWER_ENTITY_H
#define GLMODELVIEWER_ENTITY_H

#include <ecs/Transform.h>
#include <vulkan/vulkan.hpp>

#define FOR_EACH_COMPONENT(task) ForEachComponent([&](const std::shared_ptr<Component>& component){task;});

class Component;
class VulkanCore;

class Entity {
public:
    Entity();

    void Update(float delta);
    void Record(int bufferIdx, vk::CommandBuffer& buffer, VulkanCore* core);

    template<typename T>
    std::shared_ptr<T> AddComponent(std::initializer_list<T> initList) {
        std::shared_ptr<T> ptr = std::make_shared<T>(initList);
        components[typeid(T).hash_code()] = ptr;
        return ptr;
    }

    template<typename T>
    std::shared_ptr<T> GetComponent() {
        return components[typeid(T).hash_code()];
    }

    void ForEachComponent(const std::function<void(std::shared_ptr<Component>)>& func);

    void AddChild(Entity& child);

    Transform transform;
protected:
    std::unordered_map<size_t, std::shared_ptr<Component>> components;
};

#endif//GLMODELVIEWER_ENTITY_H
