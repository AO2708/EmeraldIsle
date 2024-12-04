#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 3) in mat4 instanceMatrix;
uniform mat4 VPLight;

void main() {
    gl_Position = VPLight * instanceMatrix * vec4(vertexPosition,1.0);
}
