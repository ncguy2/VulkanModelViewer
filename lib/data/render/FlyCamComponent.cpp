//
// Created by Guy on 08/07/2021.
//

#include <GLFW/glfw3.h>
#include <ecs/Entity.h>
#include <ecs/components/FlyCamComponent.h>
#include <iostream>
#include <core/VulkanCore.hpp>
#include <Logging.h>


void FlyCamComponent::Update(float delta, UpdateContext &context) {

    if(!context.GetButton(GLFW_MOUSE_BUTTON_RIGHT)) {
        lastX = context.mouseInput.x;
        lastY = context.mouseInput.y;

        if(cursorGrabbed) {
//            glfwSetInputMode(context.core->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            cursorGrabbed = false;
        }

        return;
    }

    double x;
    double y;
    glfwGetCursorPos(context.core->window, &x, &y);

    if(!cursorGrabbed) {
//        glfwSetInputMode(context.core->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        cursorGrabbed = true;
    }

    float spd = speed;
    if(context.GetKey(GLFW_KEY_LEFT_SHIFT))
        spd *= 3;
    float s = spd * delta;

    Transform* t = &attachedEntity->transform;

    glm::vec3 forward = camdir;
    glm::vec3 left = glm::cross({0, 1, 0}, forward);
    glm::vec3 up = glm::cross(forward, left);

    if(context.GetKey(GLFW_KEY_W))
        t->translation += s * forward;
    if(context.GetKey(GLFW_KEY_S))
        t->translation -= s * forward;
    if(context.GetKey(GLFW_KEY_A))
        t->translation += s * left;
    if(context.GetKey(GLFW_KEY_D))
        t->translation -= s * left;

    if(context.GetKey(GLFW_KEY_SPACE))
        t->translation += s * up;
    if(context.GetKey(GLFW_KEY_LEFT_CONTROL))
        t->translation -= s * up;

    if(isFirstFrame) {
        isFirstFrame = false;
        lastX = context.mouseInput.x;
        lastY = context.mouseInput.y;
        return;
    }

    double xoffset = context.mouseInput.x - lastX;
    double yoffset = lastY - context.mouseInput.y;
    lastX = context.mouseInput.x;
    lastY = context.mouseInput.y;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if(pitch > 89.0)
        pitch = 89.0;
    if(pitch < -89.0)
        pitch = -89.0;

    glm::vec3 direction;
    direction.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    direction.y = glm::sin(glm::radians(pitch));
    direction.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    direction = glm::normalize(direction);

//    t->SetFromEulerAngles({glm::radians(pitch), glm::radians(yaw), 0});

//    std::cout << "Direction: {" << direction.x << ", " << direction.y << ", " << direction.z << "}" << std::endl;
//    t->SetForward(direction);
    camdir = direction;
}
