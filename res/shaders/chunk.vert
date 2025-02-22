#version 330 core 
layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out vec4 out_color;
out vec3 out_normal;
out vec2 out_uv;
out vec3 FragPos;
out vec4 FragPosLightSpace;

void main(){
     gl_Position = projection * view * model * vec4(position, 1.0);
     out_color = color;
     out_normal = mat3(transpose(inverse(model))) * normal; 
     out_uv = uv;
     FragPos = vec3(model * vec4(position, 1.0));
     FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}

