#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{ 
    float depthValue = texture(screenTexture, TexCoords).r;
    FragColor = vec4(vec3(depthValue), 1.0);
}