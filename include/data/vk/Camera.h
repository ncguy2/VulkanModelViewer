//
// Created by Guy on 18/06/2021.
//

#ifndef GLMODELVIEWER_CAMERA_H
#define GLMODELVIEWER_CAMERA_H

#include <memory>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

class FBO;

struct PerspectiveSettings {
    float fov = 45.0f;
    float width = 1024.0f;
    float height = 1024.0f;

    float nearPlane = 0.1f;
    float farPlane = 100.0f;
};

struct OrthographicSettings {
    float left = 1.0f;
    float right = 1.0f;
    float bottom = 1.0f;
    float top = 1.0f;
};

enum CameraProjection {
    proj_Perspective,
    proj_Orthographic,
};

class Camera {
public:
    Camera();

    void Render(vk::Device& device, vk::Queue& queue, vk::Semaphore& waitSemaphore, vk::Semaphore& signalSemaphore, vk::Fence& fence, std::vector<vk::CommandBuffer> commandBuffers);

    glm::mat4 GetProjectionMatrix();

    void SetPerspective(PerspectiveSettings settings);
    void SetOrthographic(OrthographicSettings settings);

protected:
    CameraProjection activeProjection;
    PerspectiveSettings perspective;
    OrthographicSettings orthographic;

    glm::mat4 perspectiveMatrix{};
    glm::mat4 orthographicMatrix{};

};

#endif//GLMODELVIEWER_CAMERA_H
