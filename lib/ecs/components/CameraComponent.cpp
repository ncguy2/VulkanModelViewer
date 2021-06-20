//
// Created by Guy on 16/06/2021.
//

#include <core/VulkanCore.hpp>
#include <ecs/Entity.h>
#include <ecs/components/CameraComponent.h>
#include <data/vk/Camera.h>

void CameraComponent::Update(float delta, UpdateContext& context) {
    glm::mat4 proj = camera.GetProjectionMatrix();
    glm::vec3 position = attachedEntity->transform.translation;
    glm::vec3 direction = glm::vec3(0, 0, -1) * attachedEntity->transform.rotation;

    glm::mat4 view = glm::lookAt(position, direction, glm::vec3(0.0f, 1.0f, 0.0f));

    context.view = view;
    context.proj = proj;

    context.camera = &camera;
}
