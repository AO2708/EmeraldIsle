#version 330 core

in vec3 color;
in vec3 worldPosition;
in vec3 worldNormal;

uniform vec3 lightPosition;
uniform vec3 lightIntensity;

out vec3 finalColor;

void main() {
    vec3 gamma = vec3(2.2);
    float distance = dot(lightPosition - worldPosition,lightPosition - worldPosition);
    if (distance > 0.0) {
        vec3 worldNormal = normalize(worldNormal);
        vec3 light = normalize(lightPosition - worldPosition);
        float cosTheta = max(dot(worldNormal, light),0.0);
        float attenuation = 1.0 / (4.0 * 3.14159 * distance);
        float reflectance = 1.0 / 3.14159;
        if (color == vec3(1.0f, 1.0f, 1.0f)) {
            reflectance *= 8.0 * 0.747 + 15.6 * 0.740 + 18.4 * 0.737;
        } else if (color == vec3(1.0f, 0.0f, 0.0f)) {
            reflectance *= 8.0 * 0.058 + 15.6 * 0.287 + 18.4 * 0.642;
        } else if (color == vec3(0.0f, 1.0f, 0.0f)) {
            reflectance *= 8.0 * 0.285 + 15.6 * 0.160 + 18.4 * 0.159;
        }

        finalColor = reflectance * cosTheta * lightIntensity * attenuation * color;
    } else {
        finalColor = vec3(1.0);
    }
    finalColor = finalColor / (1 + finalColor);      // Reinhard tone mapper
    finalColor = pow(finalColor,1.0/gamma);          // gamma correction
}