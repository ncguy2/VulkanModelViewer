//
// Created by Guy on 16/06/2021.
//

#include <ecs/Entity.h>
#include <ecs/Component.h>
#include <iostream>
#include <Logging.h>

void Entity::Update(float delta, UpdateContext& context) {
    FOR_EACH_COMPONENT(component->Update(delta, context));
}

void Entity::ForEachComponent(const std::function<void(std::shared_ptr<Component>)>& func) {
    for (auto &item : components) {
        if (item.second)
            func(item.second);
        else
            Logging::Get() << "Entity has dead component reference" << std::endl;
    }
}

Entity::Entity() {
    transform.attachedEntity = this;
}

void Entity::AddChild(Entity& child) {
    child.transform.SetParent(&this->transform);
}
