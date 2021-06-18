//
// Created by Guy on 15/06/2021.
//

#ifndef GLMODELVIEWER_EXAMPLESCREEN_H
#define GLMODELVIEWER_EXAMPLESCREEN_H

#include <core/Screen.h>
#include <data/Mesh.h>

class ExampleScreen : public Screen {
public:
    void Update(float delta) override;
    void Record(int bufferIdx, vk::CommandBuffer &buffer) override;
    void Resize(int width, int height) override;
    void Show() override;
    void Hide() override;
    void Dispose() override;
    void OnKey(int key, int scancode, int action, int mods) override;

    void AddMesh(const std::shared_ptr<Mesh>& mesh);

protected:
    std::shared_ptr<ShaderProgram> shader;
    std::shared_ptr<Texture> texture;
    std::vector<std::shared_ptr<Mesh>> meshes;
    float t;
    EntityScene scene;
};

#endif//GLMODELVIEWER_EXAMPLESCREEN_H
