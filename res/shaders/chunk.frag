#version 330 core


in vec4 Color;
in vec3 Normal;
in vec2 TexCoords;
in vec3 FragPos;
in vec4 FragPosLightSpace;

out vec4 FragColor;
//texture
uniform sampler2D blockTexture;

//shadow
uniform sampler2D shadowMap;
uniform vec3 sunLightDir;

//fog
uniform vec3 worldCamera;
uniform bool enableFog;
uniform float fogNear;
uniform float fogFar;

float fogColor(){
    float dist = distance(worldCamera, FragPos);
    float fact = 1.0 * (dist - fogNear)/(fogFar - fogNear);
    return clamp(fact, 0.0, 1.0);
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    vec3 normal = normalize(Normal);
    float bias = max(0.05 * (1.0 - dot(normal, -sunLightDir)), 0.005);
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

    vec3 sun_dir = normalize(vec3(0.1,0.8,0.1));
    float diffuse = max(dot(Normal, sun_dir), 0.0);
    // //漫反射
    vec3 ambient_color = vec3(1.0,1.0,1.0);
    float ambient_strength = 0.6;
    vec3 ambient = ambient_strength * ambient_color;

    float shadow = ShadowCalculation(FragPosLightSpace);

    vec3 result = (ambient +  (1.0 - shadow) * diffuse * 0.9) ;
    vec4 color = texture(blockTexture, TexCoords) * vec4(result,1.0) * Color;

    float fog = fogColor();
    if(enableFog){
        FragColor = mix(color, vec4(0.71,0.82,1.0,1.0), fog);
    }else{
        FragColor = color;
    }
}
