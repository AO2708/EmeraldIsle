#version 330 core

in float percentageDarkBlue;
in vec2 uv;
in vec3 normal;
in vec3 worldPositionMoved;

uniform sampler2D normalMap;
uniform vec3 lightPosition;
uniform vec3 lightIntensity;
uniform vec3 cameraPosition;

out vec3 finalColor;

float lambertianTerm(vec3 normalTexture) {
    vec3 worldNormal = normalize(normalTexture);
    // Sun rays are parallel
    vec3 light = normalize(lightPosition);
    float cosTheta = dot(worldNormal, light);
    float reflectance = 1.0 / 3.14159;
    float lambTerm = reflectance * cosTheta;
    return lambTerm;
}

vec3 blinnPhong(vec3 normalTexture) {
    // Sun rays are parallel
    vec3 lightDir = normalize(lightPosition);
    vec3 viewDir = normalize(cameraPosition - worldPositionMoved);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normalize(normalTexture), halfwayDir),0.0), 20.0);
    vec3 specular = vec3(0.6, 0.8, 1.0) * spec;
    return specular;
}

void main() {
    // Normal mapping
    vec3 normalTexture = texture(normalMap, uv).rgb;
    normalTexture = normalize(normalTexture);

    // Color choice
    vec3 color;
    if(percentageDarkBlue > 1.0) {
        color = vec3(0.0, 0.2, 0.4);
    } else {
        color = (1 - percentageDarkBlue) * vec3(0.3, 0.7, 1.0) + percentageDarkBlue * vec3(0.0, 0.2, 0.4);
    }

    vec3 gamma = vec3(2.2);
    // Approximation : All the surface receive the same amount of energy
    float distance = dot(lightPosition,lightPosition);
    float lambTerm = lambertianTerm(normalTexture);
    float attenuation = 0.1 / (4.0 * 3.14159 * distance);
    vec3 diffuse = (lightIntensity*attenuation*lambTerm) * color;
    vec3 specular = blinnPhong(normalTexture);
    finalColor = diffuse + specular;

    finalColor = finalColor / (1 + finalColor);      // Reinhard tone mapper
    finalColor = pow(finalColor,1.0/gamma);          // gamma correction
}