//
// Created by Guy on 18/06/2021.
//

#include <data/vk/Camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <pch.h>
#include <utility>

Camera::Camera() {
    SetPerspective(PerspectiveSettings{});
}

float Camera::GetNear() {
    if(activeProjection == CameraProjection::proj_Perspective)
        return perspective.nearPlane;
    return 0;
}

float Camera::GetFar() {
    if(activeProjection == CameraProjection::proj_Perspective)
        return perspective.farPlane;
    return 1;
}

glm::mat4 Camera::GetProjectionMatrix() {
    if(activeProjection == CameraProjection::proj_Orthographic)
        return this->orthographicMatrix;
    return this->perspectiveMatrix;
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

void Camera::Update() {
}

