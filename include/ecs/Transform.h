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

    glm::vec3 forward() {
        return glm::vec3(0, 0, -1) * rotation;
    }

    glm::vec3 right() {
        return glm::vec3(1, 0, 0) * rotation;
    }

    glm::vec3 up() {
        return glm::vec3(0, 1, 0) * rotation;
    }

    void SetFromEulerAngles(glm::vec3 angles) {
        rotation = glm::quat(angles);
    }

    void SetForward(glm::vec3 D) {
        rotation = glm::quatLookAt(D, {0, 1, 0});

        glm::vec3 F = D;
        glm::vec3 R = glm::cross({0, 1, 0}, D);
        glm::vec3 L = -R;
        glm::vec3 U = glm::cross(F, R);

        glm::mat4 rot(glm::identity<glm::mat4>());


//        glm::vec3 v2 = glm::normalize(glm::cross({0, 1, 0}, D));
//        glm::vec3 v3 = glm::cross(D, v2);
//
//        auto m00 = v2.x;
//        auto m01 = v2.y;
//        auto m02 = v2.z;
//
//        auto m10 = v3.x;
//        auto m11 = v3.y;
//        auto m12 = v3.z;
//
//        auto m20 = D.x;
//        auto m21 = D.y;
//        auto m22 = D.z;
//
//        float num8 = m00 + m11 + m22;
//
//        if(num8 > 0.0f) {
//            float num = glm::sqrt(num8 + 1.0);
//            rotation.w = num * 0.5;
//            num = 0.5 / num;
//            rotation.x = (m12 - m21) * num;
//            rotation.y = (m20 - m02) * num;
//            rotation.z = (m01 - m10) * num;
//            return;
//        }
//        if(m00 >= m11 && m00 >= m22) {
//            float num = glm::sqrt(((1.0 + m00) - m11) - m22);
//            rotation.x = 0.5 * num;
//            num = 0.5 / num;
//            rotation.y = (m01 + m10) * num;
//            rotation.z = (m02 + m20) * num;
//            rotation.w = (m12 + m21) * num;
//            return;
//        }
//        if(m11 > m22) {
//            float num = glm::sqrt(((1.0 + m11) - m00) - m22);
//            rotation.y = 0.5 * num;
//            num = 0.5 / num;
//            rotation.x = (m10 + m01) * num;
//            rotation.z = (m21 + m12) * num;
//            rotation.w = (m20 + m02) * num;
//            return;
//        }
//
//        float num = glm::sqrt(((1.0 + m22) - m00) - m11);
//        rotation.z = 0.5 * num;
//        num = 0.5 / num;
//        rotation.x = (m20 + m02) * num;
//        rotation.y = (m21 + m12) * num;
//        rotation.w = (m01 + m10) * num;
//        return;
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
