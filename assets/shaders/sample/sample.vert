#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform UniformBufferObject {
//    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout (push_constant) uniform constants {
    mat4 model;
    vec4 data;
} PushConstants;


layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColour;
//layout (location = 2) in vec2 inUv;

layout (location = 0) out vec3 outColour;
layout (location = 1) out vec2 outUv;
layout (location = 2) out vec4 outData;

void main() {
    gl_Position = ubo.proj * ubo.view * PushConstants.model * vec4(inPosition, 1.0);
    outColour = inColour;
    outUv = inPosition.xy + 0.5;
    outData = PushConstants.data;
}