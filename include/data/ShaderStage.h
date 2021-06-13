//
// Created by Guy on 10/06/2021.
//

#ifndef GLMODELVIEWER_SHADERSTAGE_H
#define GLMODELVIEWER_SHADERSTAGE_H

#include <vulkan/vulkan.hpp>

class ShaderStage {
public:
    ShaderStage() = default;
    ShaderStage(const std::string &name, vk::ShaderStageFlagBits stageFlag, const std::vector<char> &source);

    void SetName(std::string name);
    void SetStage(vk::ShaderStageFlagBits stageFlag);
    void SetSource(std::vector<char> source);

    const vk::ShaderStageFlagBits &getStageFlag() const;
    const std::vector<char> &getSource() const;
    const std::string &getName() const;

    vk::PipelineShaderStageCreateInfo Create(vk::Device* device);
    void Cleanup(vk::Device* device);

protected:
    std::string name;
    vk::ShaderStageFlagBits stageFlag;
    std::vector<char> source;
    bool moduleActive;
    vk::ShaderModule module;
};

#endif//GLMODELVIEWER_SHADERSTAGE_H
