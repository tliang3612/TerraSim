#version 150

in vec3 vPosition;

out vec4 oFragColor;

uniform samplerCube uCubemap;
uniform vec3 uLightDirection;

void main() {
    float sunFalloff = 20.f;     
    float sunIntensity = .5f;   
    vec3 lightColor = vec3(1.0, 0.9, 0.7); 

    float diffuse = max(dot(normalize(vPosition), normalize(uLightDirection)), 0.0f); //diffuse calculations

    float sunHighlight = pow(diffuse, sunFalloff); 
    sunHighlight = smoothstep(0.0, 0.5, sunHighlight); 

    vec3 sunColor = lightColor * sunHighlight * sunIntensity;

    vec3 skyboxColor = texture(uCubemap, normalize(vPosition)).rgb;

    oFragColor = vec4(skyboxColor + sunColor, 1.0); //Add sun effect to the skybox
}