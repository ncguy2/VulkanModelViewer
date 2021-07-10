//
// Created by Guy on 16/06/2021.
//

#ifndef GLMODELVIEWER_CAMERACOMPONENT_H
#define GLMODELVIEWER_CAMERACOMPONENT_H

#include <data/vk/CommandBuffer.h>
#include <data/vk/Camera.h>
#include <ecs/Component.h>


class CameraComponent : public Component {
public:
    void Update(float delta, UpdateContext& context) override;

    Camera* GetCameraPtr();

protected:
    Camera camera;

};

#endif//GLMODELVIEWER_CAMERACOMPONENT_H
