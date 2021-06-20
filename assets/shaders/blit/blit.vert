#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColour;
layout (location = 2) in vec2 inUv;

layout (location = 0) out vec3 outColour;
layout (location = 1) out vec2 outUV;

void main() {
    gl_Position = vec4(inPosition.xyz, 1.0);
    outColour = inColour;
    outUV = inUv;
}