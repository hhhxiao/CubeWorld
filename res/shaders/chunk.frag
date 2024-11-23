#version 330 core
out vec4 FragColor;
in vec3 outColor;
in vec2 outTexCoord;
in vec3 outNormal;

in vec3 FragPos;

uniform sampler2D outTexture;



void main()
{
   // FragColor = texture(outTexture, outTexCoord);
   FragColor = vec4(0.4,0.4,0.4,1.0);
}