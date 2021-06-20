//
// Created by Guy on 20/06/2021.
//

#include <ecs/Entity.h>
#include <ecs/components/MeshComponent.h>
#include <ecs/components/MeshRendererComponent.h>
#include <data/Mesh.h>

void MeshRendererComponent::Update(float delta, UpdateContext &context) {
    std::shared_ptr<Mesh> mesh = attachedEntity->GetComponent<MeshComponent>()->mesh;
    if(mesh)
        context.PushMesh(mesh);
}

