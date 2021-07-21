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

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

layout (location = 0) out vec3 FragPos;
layout (location = 1) out vec3 Normal;

void main() {
    FragPos = (PushConstants.model * vec4(position, 1.0)).xyz;
    Normal = mat3(transpose(inverse(PushConstants.model))) * normal;

    gl_Position = ubo.proj * ubo.view * vec4(FragPos, 1.0);
}