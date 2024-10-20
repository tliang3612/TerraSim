#version 150

in vec3 iPosition;
in vec2 iTexture;

out vec3 vPosition;
out vec2 vTexture;
out vec4 vColor;

uniform mat4 uView;
uniform mat4 uProjection;
uniform sampler2D uHeightmap;


void main() {
	vec4 worldPosition = vec4(iPosition + vec3(0.0f, texture(uHeightmap, iTexture).r, 0.0f), 1.0f);
	vec4 relativePosition = uView * worldPosition;
	gl_Position = uProjection * relativePosition;
	vPosition = worldPosition.xyz;
	vTexture = iTexture;
}