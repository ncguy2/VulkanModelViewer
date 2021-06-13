//
// Created by Guy on 13/06/2021.
//

#include <pch.h>

#include <data/TextureSampler.h>
#include <core/VulkanCore.hpp>

TextureSampler::TextureSampler(VulkanCore *core, vk::Device& device) : core(core), device(&device) {}
TextureSampler::TextureSampler(VulkanCore *core, vk::Device* device) : core(core), device(device) {}

void TextureSampler::Create() {
    vk::PhysicalDeviceProperties props = core->GetPhysicalDeviceProperties();
    vk::PhysicalDeviceFeatures features = core->GetPhysicalDeviceFeatures();

    vk::SamplerCreateInfo samplerInfo{};
    samplerInfo.setMagFilter(vk::Filter::eLinear).setMinFilter(vk::Filter::eLinear);
    samplerInfo.setAddressModeU(vk::SamplerAddressMode::eRepeat).setAddressModeV(vk::SamplerAddressMode::eRepeat).setAddressModeW(vk::SamplerAddressMode::eRepeat);

    if(features.samplerAnisotropy) {
        samplerInfo.setAnisotropyEnable(VK_TRUE);
        samplerInfo.setMaxAnisotropy(props.limits.maxSamplerAnisotropy);
    }else{
        samplerInfo.setAnisotropyEnable(VK_FALSE);
        samplerInfo.setMaxAnisotropy(1.0f);
    }

    samplerInfo.setBorderColor(vk::BorderColor::eIntOpaqueBlack);
    samplerInfo.setUnnormalizedCoordinates(VK_FALSE);
    samplerInfo.setCompareEnable(VK_FALSE);
    samplerInfo.setCompareOp(vk::CompareOp::eAlways);
    samplerInfo.setMipmapMode(vk::SamplerMipmapMode::eLinear);
    samplerInfo.setMipLodBias(0.0f);
    samplerInfo.setMinLod(0.0f).setMaxLod(0.0f);

    CHECK(device->createSampler(&samplerInfo, nullptr, &sampler));
}

void TextureSampler::Dispose() {
    device->destroySampler(sampler);
}

vk::DescriptorImageInfo TextureSampler::Prepare(vk::ImageView &view) const {
    vk::DescriptorImageInfo imageInfo{};
    imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);
    imageInfo.setImageView(view);
    imageInfo.setSampler(sampler);

    return imageInfo;
}
