//
// Created by Guy on 30/06/2021.
//

#ifndef GLMODELVIEWER_SCENE2D_H
#define GLMODELVIEWER_SCENE2D_H

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

enum Touchable {
    touch_enabled,
    touch_disbled,
};

struct SceneRect {
    float x, y;
    float width, height;
    float originX, originY;
    float scaleX = 1, scaleY = 1;
    float rotation;
};

typedef glm::vec4 Colour;

#endif//GLMODELVIEWER_SCENE2D_H
