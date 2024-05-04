#version 330 core 
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 outColor;
out vec2 outTexCoord;

void main(){
     gl_Position = projection * view * model * vec4(position, 1.0);
     outColor = color;
     outTexCoord = texCoord;
}
