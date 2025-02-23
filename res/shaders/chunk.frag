#version 330 core
in vec2 TexCoords;
out vec4 FragColor;


uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;
uniform sampler2D shadowMap;

//lighting
uniform vec3 sunLightDir;
uniform mat4 lightSpaceMatrix;


//ssao
uniform bool enableSSAO;
//fog
uniform vec3 worldCamera;
uniform bool enableFog;
uniform float fogNear;
uniform float fogFar;


float fogColor(vec3 FragPos){
    float dist = distance(worldCamera, FragPos);
    float fact = 1.0 * (dist - fogNear)/(fogFar - fogNear);
    return clamp(fact, 0.0, 1.0);
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 Normal)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(Normal, -sunLightDir)), 0.005);
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}


void main()
{

    // //漫反射
    vec4 Diffuse = texture(gAlbedo, TexCoords);
    vec3 Normal = texture(gNormal ,TexCoords).rgb; 
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    
    vec4 ambient_color = vec4(1.0,1.0,1.0,1.0);
    float ambient_strength = 0.6;
    float ambientOcclusion = texture(ssao, TexCoords).r;
    if(!enableSSAO){
        ambientOcclusion = 1.0;
    }
    vec4 ambient = ambient_strength * ambient_color * Diffuse * ambientOcclusion;
    
   // float shadow = ShadowCalculation(lightSpaceMatrix * vec4(FragPos, 1.0), Normal);
    vec4 diffuse = max(dot(Normal, -sunLightDir), 0.0) * Diffuse * (1.0);
    vec4 color = diffuse * 0.7 + ambient;

    float fog = fogColor(FragPos);
    if(enableFog){
        FragColor = mix(color, vec4(0.71,0.82,1.0,1.0), fog);
    }else{
        FragColor = color;
    }
}
