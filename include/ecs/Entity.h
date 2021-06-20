//
// Created by Guy on 16/06/2021.
//

#ifndef GLMODELVIEWER_ENTITY_H
#define GLMODELVIEWER_ENTITY_H

#include <ecs/Transform.h>
#include <vulkan/vulkan.hpp>
#include <data/Contexts.h>
#include <ecs/Component.h>

#define FOR_EACH_COMPONENT(task) ForEachComponent([&](const std::shared_ptr<Component>& component){task;});

class VulkanCore;

class Entity {
public:
    Entity();

    void Update(float delta, UpdateContext& context);

    template<typename T, typename =
    std::enable_if_t<std::is_base_of_v<Component, std::remove_reference_t<T>>>>
    std::shared_ptr<T> AddComponent(std::initializer_list<T> initList) {
        std::shared_ptr<T> ptr = std::make_shared<T>(initList);
        ptr->Attach(this);
        components[typeid(T).hash_code()] = ptr;
        return ptr;
    }

    template<typename T, typename =
    std::enable_if_t<std::is_base_of_v<Component, std::remove_reference_t<T>>>>
    std::shared_ptr<T> AddComponent() {
        std::shared_ptr<T> ptr = std::make_shared<T>();
        ptr->Attach(this);
        components[typeid(T).hash_code()] = ptr;
        return ptr;
    }

    template<typename T, typename =
    std::enable_if_t<std::is_base_of_v<Component, std::remove_reference_t<T>>>>
    T* GetComponent() {
        return (T*) components[typeid(T).hash_code()].get();
    }

    void ForEachComponent(const std::function<void(std::shared_ptr<Component>)>& func);

    void AddChild(Entity& child);

    Transform transform;
protected:
    std::unordered_map<size_t, std::shared_ptr<Component>> components;
};

#endif//GLMODELVIEWER_ENTITY_H
