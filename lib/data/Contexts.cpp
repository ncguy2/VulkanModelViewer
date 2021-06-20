//
// Created by Guy on 20/06/2021.
//

#include <data/Contexts.h>

void UpdateContext::PushMesh(std::shared_ptr<Mesh> mesh) {
    meshesToRender.push_back(mesh);
}

void UpdateContext::Reset() {
    meshesToRender.clear();
}
