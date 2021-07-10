#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (location = 0) out vec4 FinalColour;

layout (location = 0) in vec4 cameraSettings;
layout (location = 2) in vec3 nearPoint;
layout (location = 3) in vec3 farPoint;
layout (location = 4) in mat4 fragView; // mat4 is made up of 4xvec4, each taking up 1 slot
layout (location = 8) in mat4 fragProj;

vec4 grid(vec3 fragPos3D, float scale, float intensity) {
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minx = min(derivative.x,  1);
    float minz = min(derivative.y,  1);
    vec4 colour = vec4(intensity, intensity, intensity, 1.0 - min(line, 1.0));
    // Z Axis
    if(fragPos3D.x > -0.1 * minx && fragPos3D.x < 0.1 * minx)
        colour.b = intensity * 5;

    // X Axis
    if(fragPos3D.z > -0.1*  minz && fragPos3D.z < 0.1 * minz)
        colour.r = intensity * 5;

    return colour;
}

float computeDepth(vec3 pos) {
    vec4 clipSpacePos = fragProj * fragView * vec4(pos, 1.0);
    return clipSpacePos.z / clipSpacePos.w;
}

float computeLinearDepth(vec3 pos) {

    float near = cameraSettings.x;
    float far = cameraSettings.y;

    float clipSpaceDepth = computeDepth(pos) * 2.0 - 1.0;
    float linearDepth = (2.0 * near * far) / (far + near - clipSpaceDepth * (far - near));
    return linearDepth / far; // Normalise
}

void main() {
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);
    gl_FragDepth = computeDepth(fragPos3D);

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = smoothstep(1, 0, pow(linearDepth, 0.1));

    float gridAScale = cameraSettings.z;
    float gridBScale = cameraSettings.z / 10;

//    FinalColour = (grid(fragPos3D, 10, 0.2) + grid(fragPos3D, 1, 0.2)) * float(t > 0);
    FinalColour = (grid(fragPos3D, gridAScale, 0.2) + grid(fragPos3D, gridBScale, 0.2)) * float(t > 0);
    FinalColour.a *= fading;

//    FinalColour = vec4(fading, fading, fading, 1.0);
}