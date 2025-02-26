#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoords;

out vec3 ViewPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;
out vec3 FragNormal;
out vec4 ExtraColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main() {
    vec3 worldPos =  vec3(model * vec4(aPosition, 1.0));
    FragPosLightSpace = lightSpaceMatrix * vec4(worldPos, 1.0);
    vec4 viewPos = view * model * vec4(aPosition, 1.0);
    ViewPos = viewPos.xyz; 
    TexCoords = aTexCoords;
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    Normal = normalMatrix * aNormal;
    FragNormal = aNormal;
    ExtraColor = aColor;
    gl_Position = projection * viewPos;
}