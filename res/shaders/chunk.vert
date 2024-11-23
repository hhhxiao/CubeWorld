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
out vec3 outNormal;

out vec3 FragPos;

void main(){
     gl_Position = projection * view * model * vec4(position, 1.0);
     outColor = color; //纹理自带颜色
     outTexCoord = texCoord; //纹理uv
     outNormal = normal; //法线
     FragPos = vec3(model * vec4(position, 1.0)); //片段世界坐标
}