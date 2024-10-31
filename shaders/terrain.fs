#version 150

// Input from the vertex shader
in vec3 vPosition;   // World position of the fragment
in vec2 vTexture;    // Texture coordinates
in vec3 vColor;      // Color passed from the vertex shader (grayscale from heightmap)
in vec3 vNormal;	// Calculated surface normal from the vertex shader

uniform vec3 uLightDirection; // Light direction. changes according to user input
uniform float uMinHeight;
uniform float uMaxHeight;
uniform sampler2D uTexture1;
uniform sampler2D uTexture2;
uniform sampler2D uTexture3;
uniform sampler2D uTexture4;


// Output to the framebuffer
out vec4 oFragColor;

vec3 BlendTextures(vec2 coordinates){
    // base texture 
    vec3 baseColor = texture(uTexture2, coordinates).rgb;

    // blend between texture1 and texture2 based on height
    baseColor = mix(baseColor, texture(uTexture1, coordinates).rgb, clamp(vPosition.y / 10.0f, 0.0f, 1.0f));

    // blend texture3 based on slope
    vec3 upVector = vec3(0.0f, 1.0f, 0.0f);
    float slopeFactor = max(0.0f, sqrt(1.0f - dot(normalize(vNormal), upVector)));
    baseColor = mix(baseColor, texture(uTexture3, coordinates).rgb, slopeFactor);

    // blend texture4 for peaks (based on height, stronger at high elevations)
    float peakFactor = smoothstep(55.0f, 60.0f, vPosition.y); // harded peakFactor for now
    baseColor = mix(baseColor, texture(uTexture4, coordinates).rgb, peakFactor);

    return baseColor;
}

void main() {
	vec3 unitNormal = normalize(vNormal); // Must normalize for dot product math calculation things
	vec3 brightness = clamp(dot(unitNormal, uLightDirection), 0.25f, 1.f) * vec3(1.f, 1.f, 1.f); // The more front facing the normal is to the light direction, the brighter. clamp between 0.3 and 1
	vec2 coordinates = vTexture * 100.0f;
    vec3 textureColor = BlendTextures(coordinates);
    oFragColor = vec4(brightness * textureColor, 1.f);
}

