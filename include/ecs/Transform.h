//
// Created by Guy on 15/06/2021.
//

#ifndef GLMODELVIEWER_TRANSFORM_H
#define GLMODELVIEWER_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>

class Entity;

class Transform {
public:
    Entity* attachedEntity;
    Transform *parent;
    std::vector<std::shared_ptr<Transform>> children;

    void SetParent(Transform *parent) {
        if (this->parent)
            this->parent->RemoveChild(this);
        this->parent = parent;
        parent->children.push_back(std::shared_ptr<Transform>(this));
    }

    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;

    glm::mat4 matrix() {
        glm::mat4 m;

        glm::vec3 axis = glm::axis(rotation);
        float angle = glm::angle(rotation);

        m = glm::translate(m, translation);
        m = glm::rotate(m, angle, axis);
        m = glm::scale(m, scale);

        if (parent)
            m = parent->matrix() * m;

        return m;
    }

protected:
    void RemoveChild(Transform *t) {
        auto r = std::remove(children.begin(), children.end(), std::shared_ptr<Transform>(t));
    }
};

#endif//GLMODELVIEWER_TRANSFORM_H
