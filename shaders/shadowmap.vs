#version 150

in vec3 iPosition;
 
uniform mat4 uLightProjection;

void main(){
    vec4 worldPosition = vec4(iPosition, 1.0f);
    gl_Position = uLightProjection * worldPosition;
}