#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;

out vec3 finalColor;

uniform vec3 lightPosition;
uniform vec3 lightIntensity;
uniform vec3 cameraPosition;

vec3 blinnPhong() {
    // Sun rays are parallel
    vec3 lightDir = normalize(lightPosition);
    vec3 viewDir = normalize(cameraPosition - worldPosition);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normalize(worldNormal), halfwayDir),0.0), 20.0);
    vec3 specular = vec3(1.0, 0.98, 0.95) * spec;
    return specular;
}

void main()
{
    // Lighting
    vec3 lightDir = normalize(lightPosition);
    float lightDist = dot(lightDir, lightDir);
    lightDir = normalize(lightDir);
    float attenuation = 1.0 / (4.0 * 3.14159 * 15000);
    vec3 diffuse = (0.8/3.14159) * attenuation * lightIntensity * clamp(dot(lightDir, worldNormal), 0.0, 1.0);
    diffuse *= vec3(0.5,0.5,0.5);
    vec3 specular = blinnPhong();
    finalColor = diffuse + specular;

    // Tone mapping
    finalColor = finalColor / (1 + finalColor);

    // Gamma correction
    finalColor = pow(finalColor, vec3(1.0 / 2.2));
}
