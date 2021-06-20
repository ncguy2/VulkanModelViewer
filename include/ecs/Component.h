//
// Created by Guy on 16/06/2021.
//

#ifndef GLMODELVIEWER_COMPONENT_H
#define GLMODELVIEWER_COMPONENT_H

#include <data/Contexts.h>

class Entity;
class VulkanCore;

class Component {
public:
    virtual void Update(float delta, UpdateContext& context) = 0;

    void Attach(Entity* entity) { attachedEntity = entity; }
    void Detach() { Attach(nullptr); }

protected:
    Entity* attachedEntity{};
};

#endif//GLMODELVIEWER_COMPONENT_H
