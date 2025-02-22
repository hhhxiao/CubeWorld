#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{   
    //shadow depth map
    // float depthValue = texture(screenTexture, TexCoords).r;
    // FragColor = vec4(vec3(depthValue), 1.0);
    
    
    //GBuffer
    //normal / color
   // FragColor = texture(screenTexture, TexCoords);
    float depthValue = texture(screenTexture, TexCoords).a;
    FragColor = vec4(vec3(depthValue), 1.0);
}