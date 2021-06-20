#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (set = 0, binding = 1) uniform sampler2D texSampler;

layout (location = 0) out vec4 FinalColour;

layout (location = 0) in vec3 outColour;
layout (location = 1) in vec2 outUV;

void main() {
    FinalColour = texture(texSampler, outUV);
//    FinalColour = vec4(outUV, 0.0, 1.0);
//    FinalColour = vec4(outColour, 1.0);
}