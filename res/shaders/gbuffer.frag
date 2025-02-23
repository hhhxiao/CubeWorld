#version 330 core

layout(location = 0) out vec4 gPositionDepth;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;
layout(location = 3) out float gShadow;

in vec3 ViewPos;
in vec4 FragPosLightSpace;
in vec3 Normal;
in vec3 FragNormal;
in vec2 TexCoords;

uniform sampler2D blockTexture;
uniform sampler2D depthMap;
uniform float near;
uniform float far;
//for shadow
uniform vec3 sunLightDir;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // 回到NDC
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    vec3 normal = normalize(FragNormal);
    float bias = max(0.05 * (1.0 - dot(normal, -sunLightDir)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(depthMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}


void main() {
    gPositionDepth.xyz = ViewPos;
    gPositionDepth.w = LinearizeDepth(gl_FragCoord.z) / far;
    gNormal = normalize(Normal);
    gShadow = ShadowCalculation(FragPosLightSpace);
    gAlbedoSpec.rgba = texture(blockTexture, TexCoords);
}