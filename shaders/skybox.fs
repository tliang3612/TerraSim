#version 330 core

in vec3 vPosition;

out vec4 oFragColor;

uniform samplerCube uCubemap;
uniform vec3 uLightDirection;

uniform float uSunFalloff;   
uniform float uSunIntensity;
uniform vec3 uSunColor;

void main() {
    float diffuse = max(dot(normalize(vPosition), normalize(uLightDirection)), 0.0f); //diffuse calculations

    float sunHighlight = pow(diffuse, uSunFalloff); 
    sunHighlight = smoothstep(0.f, 0.5f, sunHighlight); 
    float ambientSunHightlight = 0.2f; //appleid to the entire skybox

    vec3 sunColor = uSunColor * (ambientSunHightlight + sunHighlight * uSunIntensity);

    vec3 skyboxColor = texture(uCubemap, normalize(vPosition)).rgb;

    oFragColor = vec4(skyboxColor + sunColor, 1.f); //Add sun effect to the skybox
}