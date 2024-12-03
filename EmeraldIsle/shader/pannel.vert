#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 vertexUV;
layout(location = 4) in int vertexIsTextured;

uniform mat4 MVP;
uniform mat4 MVPLight;

out vec3 color;
out vec2 uv;
flat out int isTextured;
out vec3 worldPosition;
out vec3 worldNormal;
out vec4 fragPosLightSpace;

void main() {
    // Transform vertex
    gl_Position =  MVP * vec4(vertexPosition, 1.0);
    color = vertexColor;
    isTextured = vertexIsTextured;

    uv = vec2(vertexUV.x,1.0-vertexUV.y);
    worldPosition = vertexPosition;
    worldNormal = vertexNormal;

    fragPosLightSpace = MVPLight * vec4(vertexPosition, 1.0);
}