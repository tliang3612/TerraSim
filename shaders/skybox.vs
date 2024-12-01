#version 400 core

in vec3 iPosition;

out vec3 vPosition;

uniform mat4 uViewProjection;

void main() {
	vec4 worldPosition = uViewProjection * vec4(iPosition, 1.0f);
	gl_Position = worldPosition.xyww;
	vPosition = iPosition;
}