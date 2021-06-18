//
// Created by Guy on 16/06/2021.
//

#ifndef GLMODELVIEWER_COMPONENT_H
#define GLMODELVIEWER_COMPONENT_H

class Entity;
class VulkanCore;

class Component {
public:
    virtual void Update(float delta) = 0;
    virtual void Record(int bufferIdx, vk::CommandBuffer& buffer, VulkanCore* core) = 0;

    void Attach(Entity* entity) { attachedEntity = entity; }
    void Detach() { Attach(nullptr); }

protected:
    Entity* attachedEntity{};
};

#endif//GLMODELVIEWER_COMPONENT_H
