#version 150

// Input from the vertex shader
in vec3 vPosition;   // World position of the fragment
in vec2 vTexture;    // Texture coordinates
in vec3 vColor;      // Color passed from the vertex shader (grayscale from heightmap)
in vec3 vNormal;	// Calculated surface normal from the vertex shader

uniform vec3 uLightDirection; // Light direction. changes according to user input
uniform float uMinHeight;
uniform float uMaxHeight;
uniform sampler2D uBaseTexture;
uniform sampler2D uGroundTexture;
uniform sampler2D uMidGroundTexture;
uniform sampler2D uPeaksTexture;


// Output to the framebuffer
out vec4 oFragColor;

// Blend all textures together. some textures will be more prominent than others in certain heights
vec3 BlendTextures(vec2 coordinates){
    float normalizedHeight = clamp((vPosition.y - uMinHeight) / (uMaxHeight - uMinHeight), 0.0f, 1.0f);

    vec3 baseColor = texture(uBaseTexture, coordinates).rgb;
    vec3 groundColor = texture(uGroundTexture, coordinates).rgb;
    vec3 midGroundColor = texture(uMidGroundTexture, coordinates).rgb;
    vec3 peaksColor = texture(uPeaksTexture, coordinates).rgb;

    // blend ground with base
    float groundFactor = smoothstep(0.0f, 0.3f, normalizedHeight);
    vec3 currentColor = mix(baseColor, groundColor, groundFactor);

    // blend currentColor with midGround
    float midGroundFactor = smoothstep(0.3f, 0.7f, normalizedHeight);
    currentColor = mix(currentColor, midGroundColor, midGroundFactor);

    // blend current color with peaks
    float peaksFactor = smoothstep(0.7f, 1.0f, normalizedHeight); 
    currentColor = mix(currentColor, peaksColor, peaksFactor);

    return currentColor;
}

void main() {
	vec3 unitNormal = normalize(vNormal); // Must normalize for dot product math calculation things
	vec3 brightness = clamp(dot(unitNormal, uLightDirection), 0.25f, 1.f) * vec3(1.f, 1.f, 1.f); // The more front facing the normal is to the light direction, the brighter. clamp between 0.3 and 1
	vec2 coordinates = vTexture * 100.0f;
    vec3 textureColor = BlendTextures(coordinates);
    oFragColor = vec4(brightness * textureColor, 1.f);
}

