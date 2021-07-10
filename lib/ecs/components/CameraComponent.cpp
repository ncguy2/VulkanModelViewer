//
// Created by Guy on 16/06/2021.
//

#include <core/VulkanCore.hpp>
#include <data/vk/Camera.h>
#include <ecs/Entity.h>
#include <ecs/components/CameraComponent.h>
#include <ecs/components/FlyCamComponent.h>

void CameraComponent::Update(float delta, UpdateContext& context) {

    glm::mat4 proj = camera.GetProjectionMatrix();
    glm::vec3 position = attachedEntity->transform.translation;
//    glm::vec3 direction = attachedEntity->transform.forward();
    glm::vec3 direction = attachedEntity->GetComponent<FlyCamComponent>()->camdir;

    glm::vec3 cameraRight = glm::normalize(glm::cross({0, 1, 0}, direction));
    glm::vec3 cameraUp = glm::cross(direction, cameraRight);

    glm::mat4 view = glm::lookAt(position, position + direction, {0, 1, 0});

    context.view = view;
    context.proj = proj;

    context.camera = &camera;
}
Camera *CameraComponent::GetCameraPtr() {
    return &camera;
}
