#version 330 core

in vec3 color;
in vec2 uv;
flat in int isTextured;
in vec3 worldPosition;
in vec3 worldNormal;
in vec4 fragPosLightSpace;
in float coefReflectance;

uniform sampler2D textureSampler;
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

float lambertianTerm() {
    vec3 worldNormal = normalize(worldNormal);
    // Sun rays are parallel
    vec3 light = normalize(lightPosition);
    float cosTheta = max(dot(worldNormal, light),0.0);
    float reflectance = coefReflectance / 3.14159;
    float lambTerm = reflectance * cosTheta;
    return lambTerm;
}

void main() {
    if(isTextured > 0) {
        finalColor = texture(textureSampler,uv).rgb;
    } else {
        finalColor = color;
    }
    float shadow = shadowCalculation();
    vec3 gamma = vec3(2.2);
    // Approximation : All the surface receive the same amount of energy
    float distance = dot(lightPosition,lightPosition);
    float lambTerm = lambertianTerm();
    float attenuation = 1.0 / (4.0 * 3.14159 * distance);
    finalColor *= (lightIntensity*attenuation *lambTerm);
    finalColor = finalColor / (1 + finalColor);      // Reinhard tone mapper
    finalColor = pow(finalColor,1.0/gamma);          // gamma correction
    finalColor *= shadow;
}