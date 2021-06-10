//
// Created by Guy on 10/06/2021.
//

#ifndef GLMODELVIEWER_SHADERSTAGE_H
#define GLMODELVIEWER_SHADERSTAGE_H

#include <vulkan/vulkan.hpp>

class ShaderStage {
public:
    void SetSource(std::vector<char> source);
    void SetStage(vk::ShaderStageFlagBits stageFlag);
    void SetName(std::string name);

    const vk::ShaderStageFlagBits &getStageFlag() const;
    const std::vector<char> &getSource() const;
    const std::string &getName() const;

    vk::PipelineShaderStageCreateInfo Create(vk::Device* device);
    void Cleanup(vk::Device* device);

protected:
    vk::ShaderStageFlagBits stageFlag;
    std::vector<char> source;
    std::string name;
    bool moduleActive;
    vk::ShaderModule module;
};

#endif//GLMODELVIEWER_SHADERSTAGE_H
