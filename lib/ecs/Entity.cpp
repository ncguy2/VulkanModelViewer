//
// Created by Guy on 16/06/2021.
//

#include <ecs/Entity.h>
#include <ecs/Component.h>

void Entity::Update(float delta) {
    FOR_EACH_COMPONENT(component->Update(delta));
}

void Entity::Record(int bufferIdx, vk::CommandBuffer &buffer, VulkanCore* core) {
    FOR_EACH_COMPONENT(component->Record(bufferIdx, buffer, core));
}

void Entity::ForEachComponent(const std::function<void(std::shared_ptr<Component>)>& func) {
    for (auto &item : components)
        func(item.second);
}

Entity::Entity() {
    transform.attachedEntity = this;
}

void Entity::AddChild(Entity& child) {
    child.transform.SetParent(&this->transform);
}
