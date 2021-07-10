//
// Created by Guy on 16/06/2021.
//

#include <ecs/Scene.h>
#include <ecs/Entity.h>

std::shared_ptr<Entity> EntityScene::CreateEntity() {
    std::shared_ptr<Entity> e = std::make_shared<Entity>();
    e->transform.translation = glm::vec3(0, 0, 0);
    e->transform.rotation = glm::quat(0, 0, 0, 1);
    e->transform.scale = glm::vec3(1, 1, 1);
    entities.push_back(e);
    return e;
}

void EntityScene::Update(float delta, UpdateContext& context) {
    for (auto &item : entities)
        item->Update(delta, context);
}

void EntityScene::RemoveEntities() {
    entities.clear();
}
