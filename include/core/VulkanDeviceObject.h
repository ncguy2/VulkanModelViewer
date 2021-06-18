//
// Created by Guy on 13/06/2021.
//

#ifndef GLMODELVIEWER_VULKANDEVICEOBJECT_H
#define GLMODELVIEWER_VULKANDEVICEOBJECT_H

#include <vulkan/vulkan.hpp>
class VulkanDeviceObject {
public:
    explicit VulkanDeviceObject(vk::Device *device) : device(device) {}

    vk::Device* device;
};

#endif//GLMODELVIEWER_VULKANDEVICEOBJECT_H
