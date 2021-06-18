//
// Created by Guy on 16/06/2021.
//

#include <ecs/Scene.h>
#include <ecs/Entity.h>

std::shared_ptr<Entity> EntityScene::CreateEntity() {
    std::shared_ptr<Entity> e = std::make_shared<Entity>();
    entities.push_back(e);
    return e;
}

void EntityScene::Update(float delta) {
    for (auto &item : entities)
        item->Update(delta);
}

void EntityScene::Record(uint32_t idx, vk::CommandBuffer &buffer, VulkanCore *core) {
    for (auto &item : entities)
        item->Record(idx, buffer, core);
}
