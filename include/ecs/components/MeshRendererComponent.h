//
// Created by Guy on 20/06/2021.
//

#ifndef GLMODELVIEWER_MESHRENDERERCOMPONENT_H
#define GLMODELVIEWER_MESHRENDERERCOMPONENT_H

#include <ecs/Component.h>

class MeshRendererComponent : public Component {
public:
    void Update(float delta, UpdateContext &context) override;
};

#endif//GLMODELVIEWER_MESHRENDERERCOMPONENT_H
