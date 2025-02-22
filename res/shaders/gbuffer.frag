#version 330 core

layout(location = 0) out vec4 gPositionDepth;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D blockTexture;
uniform float near;
uniform float far;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // 回到NDC
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main() {
    gPositionDepth.xyz = FragPos;
    gPositionDepth.a =  LinearizeDepth(gl_FragCoord.z) / far;
    gNormal = normalize(Normal);
    gAlbedoSpec.rgba = texture(blockTexture, TexCoords);
}