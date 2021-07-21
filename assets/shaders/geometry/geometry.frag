#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) out vec4 FinalColour;

layout (location = 0) in vec3 FragPos;
layout (location = 1) in vec3 Normal;

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

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lighting.lightPos.xyz - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lighting.colour.rgb;

    vec3 viewDir = normalize(lighting.viewPos.xyz - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = lighting.specularStrength * spec * lighting.colour.rgb;

    return ambient + diffuse + specular;
}

void main() {
    FinalColour = vec4(CalculateLighting(), 1.0);
}