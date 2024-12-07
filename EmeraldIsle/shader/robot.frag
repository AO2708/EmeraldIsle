#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec4 fragPosLightSpace;
in vec2 uv;

out vec3 finalColor;

uniform sampler2D textureSampler;
uniform vec3 lightPosition;
uniform vec3 lightIntensity;
uniform sampler2D shadowTexture;

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

void main()
{
    vec3 color = texture(textureSampler,uv).rgb;
    float shadow = shadowCalculation();
    // Lighting
    vec3 lightDir = normalize(lightPosition);
    float lightDist = dot(lightDir, lightDir);
    lightDir = normalize(lightDir);
    float attenuation = 1.0 / (4.0 * 3.14159 * 15000);
    vec3 diffuse = (0.3/3.14159) * attenuation * lightIntensity * clamp(dot(lightDir, worldNormal), 0.0, 1.0);
    diffuse *= color;
    finalColor = diffuse;

    // Tone mapping
    finalColor = finalColor / (1 + finalColor);

    // Gamma correction
    finalColor = pow(finalColor, vec3(1.0 / 2.2));
    finalColor *= shadow;
}
