#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec4 cameraData;
} ubo;

layout (push_constant) uniform constants {
    mat4 model;
    vec4 data;
} PushConstants;


layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUv;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec2 outUv;
layout (location = 2) out vec4 outData;
layout (location = 3) out vec3 outPosition;

void main() {
    vec4 p = PushConstants.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * p;
    outPosition = p.xyz;
    outNormal = mat3(transpose(inverse(PushConstants.model))) * inNormal;
    outUv = inUv;
    outData = PushConstants.data;
}