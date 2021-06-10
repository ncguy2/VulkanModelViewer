//
// Created by Guy on 10/06/2021.
//

#include <data/ShaderStage.h>

void ShaderStage::SetSource(std::vector<char> source) {
    this->source = source;
}

void ShaderStage::SetStage(vk::ShaderStageFlagBits stageFlag) {
    this->stageFlag = stageFlag;
}

void ShaderStage::SetName(std::string name) {
    this->name = name;
}

const vk::ShaderStageFlagBits &ShaderStage::getStageFlag() const { return stageFlag; }
const std::vector<char> &ShaderStage::getSource() const { return source; }
const std::string &ShaderStage::getName() const { return name; }

vk::PipelineShaderStageCreateInfo ShaderStage::Create(vk::Device* device) {

    if(moduleActive) {
        device->destroyShaderModule(module);
        moduleActive = false;
    }

    vk::ShaderModuleCreateInfo createInfo{};
    createInfo.setCodeSize(source.size());
    createInfo.setPCode(reinterpret_cast<const uint32_t *>(source.data()));

    if(device->createShaderModule(&createInfo, nullptr, &module) != vk::Result::eSuccess)
        throw std::runtime_error("Failed to create shader module");

    moduleActive = true;

    vk::PipelineShaderStageCreateInfo stageCreateInfo{};
    stageCreateInfo.setStage(stageFlag);
    stageCreateInfo.setModule(module);
    stageCreateInfo.setPName(name.c_str());

    return stageCreateInfo;
}

void ShaderStage::Cleanup(vk::Device *device) {
    if(!moduleActive)
        return;
    device->destroyShaderModule(module);
    moduleActive = false;
}
