#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 0) uniform UniformBufferObject {
//    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 cameraData;
} ubo;

layout (location = 0) out vec4 cameraSettings;
layout (location = 2) out vec3 nearPoint;
layout (location = 3) out vec3 farPoint;
layout (location = 4) out mat4 fragView; // mat4 is made up of 4xvec4, each taking up 1 slot
layout (location = 8) out mat4 fragProj;

vec3 gridPlane[6] = vec3[](
    vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);

vec3 UnprojectPoint(float x, float y, float z, mat4 view, mat4 projection) {
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint = viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    vec3 p = gridPlane[gl_VertexIndex];

    nearPoint = UnprojectPoint(p.x, p.y, 0.0, ubo.view, ubo.proj);
    farPoint  = UnprojectPoint(p.x, p.y, 1.0, ubo.view, ubo.proj);

    fragView = ubo.view;
    fragProj = ubo.proj;

    cameraSettings = ubo.cameraData;

    gl_Position = vec4(p, 1.0);
//    gl_Position = ubo.proj * ubo.view * vec4(p, 1.0);

//    gl_Position = ubo.proj * ubo.view * vec4(gridPlane[gl_VertexIndex].xyz, 1.0);
}