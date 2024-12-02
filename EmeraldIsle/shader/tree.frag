#version 330 core

in vec3 color;
in vec3 worldPosition;
in vec3 worldNormal;
in vec4 fragPosLightSpace;

uniform vec3 lightPosition;
uniform vec3 lightIntensity;
uniform sampler2D shadowTexture;

out vec3 finalColor;

float shadowCalculation() {
    vec3 projCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float shadow;
    if(projCoords.x < 0 || projCoords.x > 1 || projCoords.y < 0 || projCoords.y > 1 || projCoords.z < 0 || projCoords.z > 1) {
        shadow = 1.0;
    } else {
        // Shadow
        float existingDepth = texture(shadowTexture, projCoords.xy).r;
        float depth = projCoords.z;
        shadow = (depth >= existingDepth + 1e-3) ? 0.2 : 1.0;
    }
    return shadow;
}

vec3 lambertianTerm() {
    vec3 worldNormal = normalize(worldNormal);
    vec3 light = normalize(lightPosition - worldPosition);
    float cosTheta = max(dot(worldNormal, light),0.0);
    float reflectance = 1.0 / 3.14159;
    if (color == vec3(1.0f, 1.0f, 1.0f)) {
        reflectance *= 8.0 * 0.747 + 15.6 * 0.740 + 18.4 * 0.737;
    } else if (color == vec3(1.0f, 0.0f, 0.0f)) {
        reflectance *= 8.0 * 0.058 + 15.6 * 0.287 + 18.4 * 0.642;
    } else if (color == vec3(0.0f, 1.0f, 0.0f)) {
        reflectance *= 8.0 * 0.285 + 15.6 * 0.160 + 18.4 * 0.159;
    }
    vec3 lambTerm = reflectance * cosTheta * color;
    return lambTerm;
}

void main() {
    float shadow = shadowCalculation();
    vec3 gamma = vec3(2.2);
    float distance = dot(lightPosition - worldPosition,lightPosition - worldPosition);
    if (distance > 0.0) {
        vec3 lambTerm = lambertianTerm();
        float attenuation = 1.0 / (4.0 * 3.14159 * distance);
        finalColor = (lightIntensity*attenuation) * (lambTerm);

    } else {
        finalColor = vec3(1.0);
    }
    finalColor = finalColor / (1 + finalColor);      // Reinhard tone mapper
    finalColor = pow(finalColor,1.0/gamma);          // gamma correction
    finalColor *= shadow;
}