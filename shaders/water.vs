#version 400 core

in vec3 iPosition;
in vec2 iTextureCoords;

out vec2 vTextureCoords;
out vec4 vClipSpace;
out vec3 vVertexToCamera;

uniform float uWaterHeight;
uniform mat4 uViewProjection;
uniform vec3 uCameraPosition;
uniform vec3 uLightDirection;


void main(void) {
	vec4 worldPosition = vec4(vec3(iPosition.x, uWaterHeight, iPosition.z), 1.0f);
	vClipSpace = uViewProjection * worldPosition;
	gl_Position = vClipSpace;
	vTextureCoords = iTextureCoords;
	vVertexToCamera = uCameraPosition - worldPosition.xyz;
}
