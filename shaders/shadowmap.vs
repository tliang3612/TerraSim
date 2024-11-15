#version 150

in vec3 iPosition;
 
uniform mat4 lightProjection;

void main(){
    vec4 worldPosition = vec4(iPosition, 1.0f);
    gl_Position = lightProjection * worldPosition;
}