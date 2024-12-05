#version 330 core

// Input
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;

uniform mat4 VP;
uniform mat4 modelMatrix;
uniform vec3 cameraPosition;
uniform float time;

out vec3 worldPositionMoved;
out vec2 uv;
out vec3 normal;
out float percentageDarkBlue;

void main() {
    normal = mat3(transpose(inverse(modelMatrix))) * vertexNormal;
    uv = vertexUV;

    // Infinite scene
    vec3 worldPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));
    worldPositionMoved = worldPosition;
    worldPositionMoved.xz += cameraPosition.xz;

    // Add waves if we are far from the island
    float distanceOrigin = length(worldPositionMoved);
    percentageDarkBlue = distanceOrigin / 150.0;
    if(distanceOrigin > 110) {
        // Waves
        float wave = sin(worldPosition.x * 5.0 + worldPosition.z * 5.0 + time);
        worldPositionMoved.y = worldPositionMoved.y + 2.0 * wave;
    }
    // Transform vertex
    gl_Position =  VP * vec4(worldPositionMoved, 1.0);
}