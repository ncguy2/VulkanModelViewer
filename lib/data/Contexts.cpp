//
// Created by Guy on 20/06/2021.
//

#include <data/Contexts.h>
#include <core/VulkanCore.hpp>
#include <GLFW/glfw3.h>

void UpdateContext::PushMesh(std::shared_ptr<Mesh> mesh) {
    meshesToRender.push_back(mesh);
}

void UpdateContext::Reset() {
    meshesToRender.clear();
}

bool UpdateContext::GetKey(int keycode) {
    return core->IsKeyPressed(keycode);
//    return glfwGetKey(core->window, keycode) == GLFW_PRESS;
}

bool UpdateContext::GetButton(int button) {
    return core->IsButtonPressed(button);
//    return glfwGetMouseButton(core->window, button) == GLFW_PRESS;
}
