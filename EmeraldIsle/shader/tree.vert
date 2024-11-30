#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 MVP;

out vec3 finalColor;

void main() {
    // Transform vertex
    gl_Position =  MVP * vec4(vertexPosition, 1.0);
    finalColor = vertexColor;
}