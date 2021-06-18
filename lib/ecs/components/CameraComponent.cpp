//
// Created by Guy on 16/06/2021.
//

#include <core/VulkanCore.hpp>
#include <ecs/Entity.h>
#include <ecs/components/CameraComponent.h>

void CameraComponent::Update(float delta) {

}

void CameraComponent::Record(int bufferIdx, vk::CommandBuffer &buffer, VulkanCore* core) {

    glm::vec3 position = attachedEntity->transform.translation;
    glm::vec3 direction = glm::vec3(0, 0, -1) * attachedEntity->transform.rotation;

    glm::mat4 view = glm::lookAt(position, direction, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 proj = glm::perspective(glm::radians(fov), width / height, near, far);
    proj[1][1] *= -1;
    core->SetViewProj(bufferIdx, view, proj);
}
