#version 150

in vec3 iPosition;
in vec2 iTexture;

out vec3 vPosition;
out vec2 vTexture;
out vec3 vColor;
out vec3 vNormal;

uniform vec4 uClip;
uniform mat4 uViewProjection;
uniform sampler2D uHeightmap;

// Calculate the surface normal of the heightmap texture. To be used for lighting
vec3 CalculateSurfaceNormal(vec2 uv) {
    vec2 epsilon = vec2(1.0 / textureSize(uHeightmap, 0).x, 1.0 / textureSize(uHeightmap, 0).y); // Small offset to be used to sample neighbors
	float down = texture(uHeightmap, uv - vec2(0.0, epsilon.y)).r; 
    float up = texture(uHeightmap, uv + vec2(0.0, epsilon.y)).r;
    float left = texture(uHeightmap, uv - vec2(epsilon.x, 0.0)).r;
    float right = texture(uHeightmap, uv + vec2(epsilon.x, 0.0)).r;
	vec3 surfaceNormal = vec3(left - right, 2.0, down - up);
    return surfaceNormal;
}

void main() {
	vec4 worldPosition = vec4(iPosition + vec3(0.0f, texture(uHeightmap, iTexture).r, 0.0f), 1.0f);
	gl_Position =  uViewProjection * worldPosition;
	vPosition = worldPosition.xyz;
	vTexture = iTexture;
	vNormal = CalculateSurfaceNormal(iTexture);
	float height = texture(uHeightmap, iTexture).r;
	float grayscaleIntensity = clamp((height - (-60)) / (60 - (-60)), 0.0, 1.0); //hardcoded for now
	vec3 grayscaleColor = vec3(grayscaleIntensity); 
	vColor = grayscaleColor;
}