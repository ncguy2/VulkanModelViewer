//
// Created by Guy on 08/07/2021.
//

#ifndef GLMODELVIEWER_FLYCAMCOMPONENT_H
#define GLMODELVIEWER_FLYCAMCOMPONENT_H

#include <ecs/Component.h>
#include <glm/glm.hpp>

class FlyCamComponent : public Component {
public:
    void Update(float delta, UpdateContext &context) override;

    float speed = 1.0f;
    float sensitivity = 0.1f;

    glm::vec3 camdir;

protected:
    bool isFirstFrame = true;
    double lastX = 0;
    double lastY = 0;

    double yaw;
    double pitch;
    bool cursorGrabbed = false;
};

#endif//GLMODELVIEWER_FLYCAMCOMPONENT_H
