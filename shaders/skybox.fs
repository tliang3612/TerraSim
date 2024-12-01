#version 400 core

in vec3 vPosition;

out vec4 oFragColor;

uniform samplerCube uCubemap;
uniform vec3 uLightDirection;

uniform float uSunFalloff;   
uniform float uSunIntensity;

void main() {
    vec3 sunlightColor = vec3(1.f, 0.9f, 0.7f); 

    float diffuse = max(dot(normalize(vPosition), normalize(uLightDirection)), 0.0f); //diffuse calculations

    float sunHighlight = pow(diffuse, uSunFalloff); 
    sunHighlight = smoothstep(0.f, 0.5f, sunHighlight); 

    vec3 sunColor = sunlightColor * sunHighlight * uSunIntensity;

    vec3 skyboxColor = texture(uCubemap, normalize(vPosition)).rgb;

    oFragColor = vec4(skyboxColor + sunColor, 1.f); //Add sun effect to the skybox
}