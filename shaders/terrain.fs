#version 150

// Input from the vertex shader
in vec3 vPosition;   // World position of the fragment
in vec2 vTexture;    // Texture coordinates
in vec4 vColor;      // Color passed from the vertex shader (grayscale from heightmap)

// Output to the framebuffer
out vec4 oFragColor;

void main() {
    oFragColor = vec4(.5,.5,.5,1);
}