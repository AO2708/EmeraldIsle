#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;

uniform mat4 MVP;
uniform mat4 MVPLight;

out vec3 color;
out vec3 worldPosition;
out vec3 worldNormal;
out vec4 fragPosLightSpace;
out float coefReflectance;

float calculationReflectance() {
    float tolerance = 0.2;
    float dominanceFactor = 2.0;
    float diffRG = abs(vertexColor.r - vertexColor.g);
    float diffGB = abs(vertexColor.g - vertexColor.b);
    float diffBR = abs(vertexColor.b - vertexColor.r);
    if (diffRG < tolerance && diffGB < tolerance && diffBR < tolerance) {
        return 0.15;
    } else if (vertexColor.g > vertexColor.r * dominanceFactor && vertexColor.g > vertexColor.b * dominanceFactor) {
        return 0.3;
    } else {
        return 0.4;
    }
}

void main() {
    // Transform vertex
    gl_Position =  MVP * vec4(vertexPosition, 1.0);
    color = vertexColor;

    worldPosition = vertexPosition;
    worldNormal = vertexNormal;
    fragPosLightSpace = MVPLight * vec4(vertexPosition, 1.0);
    coefReflectance = calculationReflectance();
}