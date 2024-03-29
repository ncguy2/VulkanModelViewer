#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (set = 0, binding = 1) uniform sampler2D texSampler;

layout (location = 0) out vec4 FinalColour;
layout (location = 0) in vec3 outNormal;
layout (location = 1) in vec2 outUv;
layout (location = 2) in vec4 outData;
layout (location = 3) in vec3 outPosition;

layout (push_constant) uniform LightParameters {
    mat4 model; // Padding
    vec4 data;  // Padding 2
    vec4 lightPos;
    vec4 viewPos;
    vec4 colour;
    float ambientStrength;
    float specularStrength;
} lighting;

vec3 CalculateLighting() {
    vec3 ambient = lighting.ambientStrength * lighting.colour.rgb;

    vec3 norm = normalize(outNormal);
    vec3 lightDir = normalize(lighting.lightPos.xyz - outPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lighting.colour.rgb;

    vec3 viewDir = normalize(lighting.viewPos.xyz - outPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = lighting.specularStrength * spec * lighting.colour.rgb;

    return ambient + diffuse + specular;
}

void main() {
    FinalColour = vec4(CalculateLighting(), 1.0) * texture(texSampler, outUv);
}