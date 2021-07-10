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

#define C_GET_KEY(T) typeid(T).hash_code()

class Entity {
public:
    Entity();

    void Update(float delta, UpdateContext& context);

    template<typename T, typename =
    std::enable_if_t<std::is_base_of_v<Component, std::remove_reference_t<T>>>>
    bool HasComponent() {
        return components.count(C_GET_KEY(T));
    }

    template<typename T, typename =
    std::enable_if_t<std::is_base_of_v<Component, std::remove_reference_t<T>>>>
    std::shared_ptr<T> AddComponent(std::initializer_list<T> initList) {
        std::shared_ptr<T> ptr = std::make_shared<T>(initList);
        ptr->Attach(this);
        components[C_GET_KEY(T)] = ptr;
        return ptr;
    }

    template<typename T, typename =
    std::enable_if_t<std::is_base_of_v<Component, std::remove_reference_t<T>>>>
    std::shared_ptr<T> AddComponent() {
        std::shared_ptr<T> ptr = std::make_shared<T>();
        ptr->Attach(this);
        components[C_GET_KEY(T)] = ptr;
        return ptr;
    }

    template<typename T, typename =
    std::enable_if_t<std::is_base_of_v<Component, std::remove_reference_t<T>>>>
    T* GetComponent() {
        if(HasComponent<T>())
            return (T*) components[C_GET_KEY(T)].get();
        return nullptr;
    }

    void ForEachComponent(const std::function<void(std::shared_ptr<Component>)>& func);

    void AddChild(Entity& child);

    Transform transform;
protected:
    std::unordered_map<size_t, std::shared_ptr<Component>> components;
};

#endif//GLMODELVIEWER_ENTITY_H
