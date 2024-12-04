#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in mat4 instanceMatrix;

uniform mat4 VP;
uniform mat4 VPLight;

out vec3 color;
out vec3 worldPosition;
out vec3 worldNormal;
out vec4 fragPosLightSpace;
out float coefReflectance;

float calculationReflectance() {
    if (vertexColor == vec3(0.184474, 0.061246, 0.034340)) {
        return 0.25;
    } else {
        return 0.30;
    }
}

void main() {
    // Transform vertex
    gl_Position =  VP * instanceMatrix * vec4(vertexPosition, 1.0);
    color = vertexColor;

    worldPosition = vec3(instanceMatrix * vec4(vertexPosition, 1.0));
    worldNormal = mat3(transpose(inverse(instanceMatrix))) * vertexNormal;

    fragPosLightSpace = VPLight * instanceMatrix * vec4(vertexPosition, 1.0);
    coefReflectance = calculationReflectance();
}