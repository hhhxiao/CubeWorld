#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
void main()
{   
    // //SSAO
    float v = texture(screenTexture, TexCoords).r;
    FragColor = vec4(v,v,v,1.0);
}