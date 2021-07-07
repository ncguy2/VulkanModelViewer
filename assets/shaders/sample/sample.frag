#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (set = 0, binding = 1) uniform sampler2D texSampler;

layout (location = 0) out vec4 FinalColour;
layout (location = 0) in vec3 outNormal;
layout (location = 1) in vec2 outUv;
layout (location = 2) in vec4 outData;

//vec3 hsv2rgb(vec3 c) {
//    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
//    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
//    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
//}

void main() {
    FinalColour = texture(texSampler, outUv);
}