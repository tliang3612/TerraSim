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
vec3 BlendTextures(vec2 texScale){
    vec3 unitNormal = normalize(vNormal);

    vec3 baseColor = texture(uBaseTexture, texScale).rgb;
    vec3 groundColor = texture(uGroundTexture, texScale).rgb;
    vec3 midGroundColor = texture(uMidGroundTexture, texScale).rgb;
    vec3 peaksColor = texture(uPeaksTexture, texScale).rgb;


    // blend base with midGround
    float epsilon = 0.0001f; // remove divide by 0 case
    float threshold = uMinHeight + 80.f;
    float normalizedMinHeight = clamp((vPosition.y - uMinHeight) / (threshold - uMinHeight + epsilon), 0.0f, 1.0f);
    float midGroundFactor = smoothstep(0.2, 0.7, normalizedMinHeight);
    vec3 currentColor = mix(baseColor, midGroundColor, midGroundFactor);

    // blend current color with ground color
    vec3 upVector = vec3(0.0f, 1.0f, 0.0f);
    float groundFactor = max(0.0f, 1.0f - dot(unitNormal, upVector));
    currentColor = mix(currentColor, groundColor, groundFactor);

    // blend current color with peaks
    float normalizedMaxHeight = clamp((vPosition.y + uMaxHeight) / (2.0 * uMaxHeight), 0.0, 1.0);
    float peaksFactor = smoothstep(0.7f, 1.0f, normalizedMaxHeight); 
    currentColor = mix(currentColor, peaksColor, peaksFactor);

    return currentColor;
}

void main() {
	vec3 unitNormal = normalize(vNormal); // Must normalize for dot product math calculation things
	vec3 brightness = clamp(dot(unitNormal, uLightDirection), 0.25f, 1.f) * vec3(1.f, 1.f, 1.f); // The more front facing the normal is to the light direction, the brighter. clamp between 0.3 and 1
	vec2 texScale = vTexture * 200.0f;
    vec3 textureColor = BlendTextures(texScale);
    oFragColor = vec4(brightness * textureColor, 1.f);
}

