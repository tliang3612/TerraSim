#version 330 core

in vec3 iPosition;
in vec2 iTextureCoords;

out vec3 vPosition;
out vec2 vTextureCoords;
out vec3 vColor;
out vec3 vNormal;
out vec4 vFragPositionLight;

uniform vec4 uClip;
uniform mat4 uViewProjection;
uniform float uTextureScale;
uniform float uMinHeight;
uniform float uMaxHeight;
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
	vec4 worldPosition = vec4(iPosition + vec3(0.f, texture(uHeightmap, iTextureCoords).r, 0.f), 1.f);
	gl_Position =  uViewProjection * worldPosition;
	gl_ClipDistance[0] = dot(worldPosition, uClip);
	vPosition = worldPosition.xyz;
	vTextureCoords = iTextureCoords * uTextureScale;
	vNormal = CalculateSurfaceNormal(iTextureCoords);
}