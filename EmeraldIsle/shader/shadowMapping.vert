#version 330 core

layout(location = 0) in vec3 vertexPosition;
uniform mat4 MVPLight;

void main() {
    gl_Position = MVPLight * vec4(vertexPosition,1.0);
}
