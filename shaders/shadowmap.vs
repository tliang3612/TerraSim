#version 150

in vec3 iPosition;

out vec3 vPosition;
 
uniform mat4 lightProjection;

void main(){
    vPosition = iPosition;
    vec4 worldPosition = vec4(iPosition, 1.0f);
    gl_Position = lightProjection * worldPosition;
}