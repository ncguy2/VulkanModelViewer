//
// Created by Guy on 20/06/2021.
//

#ifndef GLMODELVIEWER_MESHCOMPONENT_H
#define GLMODELVIEWER_MESHCOMPONENT_H

#include <ecs/Component.h>

class Mesh;

class MeshComponent : public Component {
public:
    void Update(float delta, UpdateContext &context) override;
    std::shared_ptr<Mesh> mesh;
};

#endif//GLMODELVIEWER_MESHCOMPONENT_H
