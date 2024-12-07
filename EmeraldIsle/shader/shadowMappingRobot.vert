#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 3) in vec4 joint;  // Joints
layout(location = 4) in vec4 weight; // Poids

uniform mat4 u_jointMatrix[13];
uniform mat4 vpLightMatrix;
uniform mat4 modelMatrix;

void main() {
    mat4 skinMatrix =
    weight.x * u_jointMatrix[int(joint.x)] +
    weight.y * u_jointMatrix[int(joint.y)] +
    weight.z * u_jointMatrix[int(joint.z)] +
    weight.w * u_jointMatrix[int(joint.w)];
    gl_Position = vpLightMatrix * modelMatrix * skinMatrix * vec4(vertexPosition,1.0);
}
