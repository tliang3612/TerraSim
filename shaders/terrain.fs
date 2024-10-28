#version 150

// Input from the vertex shader
in vec3 vPosition;   // World position of the fragment
in vec2 vTexture;    // Texture coordinates
in vec3 vColor;      // Color passed from the vertex shader (grayscale from heightmap)
in vec3 vNormal;	// Calculated surface normal from the vertex shader

uniform vec3 uLightDirection; // Light direction. changes according to user input

// Output to the framebuffer
out vec4 oFragColor;

void main() {
	vec3 unitNormal = normalize(vNormal); // Must normalize for dot product math calculation things
	vec3 brightness = clamp(dot(unitNormal, uLightDirection), 0.25f, 1.f) * vec3(1.f, 1.f, 1.f); // The more front facing the normal is to the light direction, the brighter. clamp between 0.3 and 1
    oFragColor = vec4(brightness * vColor, 1.f);
}

