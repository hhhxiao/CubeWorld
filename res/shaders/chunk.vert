#version 330 core 
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out vec4 Color;
out vec3 Normal;
out vec2 TexCoords;
out vec3 FragPos;
out vec4 FragPosLightSpace;

void main(){
     gl_Position = projection * view * model * vec4(aPosition, 1.0);
     Color = aColor;
     Normal = mat3(transpose(inverse(model))) * aNormal; 
     TexCoords = aTexCoords;
     FragPos = vec3(model * vec4(aPosition, 1.0));
     FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}

