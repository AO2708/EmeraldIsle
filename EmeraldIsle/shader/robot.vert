#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec4 joint;  // Joints
layout(location = 4) in vec4 weight; // Poids

uniform mat4 u_jointMatrix[13];


// Output data, to be interpolated for each fragment
out vec3 worldPosition;
out vec3 worldNormal;
out vec4 fragPosLightSpace;
out vec2 uv;

uniform mat4 VP;
uniform mat4 VPLight;
uniform mat4 modelMatrix;

void main() {
    mat4 skinMatrix =
    weight.x * u_jointMatrix[int(joint.x)] +
    weight.y * u_jointMatrix[int(joint.y)] +
    weight.z * u_jointMatrix[int(joint.z)] +
    weight.w * u_jointMatrix[int(joint.w)];

    // Transform vertex
    gl_Position = VP * modelMatrix * skinMatrix * vec4(vertexPosition, 1.0);
    fragPosLightSpace = VPLight * modelMatrix * skinMatrix * vec4(vertexPosition, 1.0);

    // Transform to world-space for lighting calculations
    worldPosition = vec3(modelMatrix * skinMatrix * vec4(vertexPosition, 1.0));

    // World-space geometry
    worldNormal = mat3(transpose(inverse(modelMatrix * skinMatrix))) * vertexNormal;

    uv = vertexUV;
}
