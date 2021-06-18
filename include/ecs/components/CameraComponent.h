//
// Created by Guy on 16/06/2021.
//

#ifndef GLMODELVIEWER_CAMERACOMPONENT_H
#define GLMODELVIEWER_CAMERACOMPONENT_H

#include <data/vk/CommandBuffer.h>
#include <ecs/Component.h>

class CameraComponent : public Component {
public:
    void Update(float delta) override;
    void Record(int bufferIdx, vk::CommandBuffer &buffer, VulkanCore* core) override;

    float fov = 45.0f;
    float width = 1024;
    float height = 1024;
    float near = 0.1f;
    float far = 100.0f;

};

#endif//GLMODELVIEWER_CAMERACOMPONENT_H
