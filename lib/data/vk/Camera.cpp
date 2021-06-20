//
// Created by Guy on 18/06/2021.
//

#include <data/vk/Camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <pch.h>
#include <utility>

glm::mat4 Camera::GetProjectionMatrix() {
    if(activeProjection == CameraProjection::proj_Orthographic)
        return this->orthographicMatrix;
    return this->perspectiveMatrix;
}

void Camera::Render(vk::Device& device, vk::Queue &queue, vk::Semaphore &waitSemaphore, vk::Semaphore &signalSemaphore, vk::Fence &fence, std::vector<vk::CommandBuffer> commandBuffers) {
    vk::SubmitInfo submitInfo{};

    vk::PipelineStageFlags waitStages[] = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    submitInfo.setWaitSemaphoreCount(1);
    submitInfo.setPWaitSemaphores(&waitSemaphore);
    submitInfo.setPWaitDstStageMask(waitStages);

    submitInfo.setSignalSemaphoreCount(1);
    submitInfo.setPSignalSemaphores(&signalSemaphore);

    submitInfo.setCommandBuffers(commandBuffers);

    CHECK(device.resetFences(1, &fence));
    CHECK(queue.submit(1, &submitInfo, fence));
}

void Camera::SetPerspective(PerspectiveSettings settings) {
    perspective = settings;
    activeProjection = CameraProjection::proj_Perspective;

    perspectiveMatrix = glm::perspective(glm::radians(settings.fov), settings.width / settings.height, settings.nearPlane, settings.farPlane);
    perspectiveMatrix[1][1] *= -1;
}

void Camera::SetOrthographic(OrthographicSettings settings) {
    orthographic = settings;
    activeProjection = CameraProjection::proj_Orthographic;

    orthographicMatrix = glm::ortho(settings.left, settings.right, settings.bottom, settings.top);
}

Camera::Camera() {
    SetPerspective(PerspectiveSettings{});
}
