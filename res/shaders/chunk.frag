#version 330 core
in vec2 TexCoords;
out vec4 FragColor;


uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D shadowMap;
uniform sampler2D ssao;

//lighting
uniform vec3 sunLightDir;


//ssao
uniform bool enableSSAO;
//fog
uniform vec3 worldCamera;
uniform bool enableFog;
uniform float fogNear;
uniform float fogFar;
uniform float ambient_strength = 1.0;
uniform float diffuse_strength = 0.6;

float fogColor(vec3 FragPos){
    float dist = distance(worldCamera, FragPos);
    float fact = 1.0 * (dist - fogNear)/(fogFar - fogNear);
    return clamp(fact, 0.0, 1.0);
}

void main()
{

    // //漫反射
    vec4 Diffuse = texture(gAlbedo, TexCoords);
    vec3 Normal = texture(gNormal ,TexCoords).rgb; 
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    float shadow = texture(shadowMap, TexCoords).r;

    vec4 ambient_color = vec4(1.0,1.0,1.0,1.0);
    float ambientOcclusion = texture(ssao, TexCoords).r;
    if(!enableSSAO){
        ambientOcclusion = 1.0;
    }
    vec4 ambient = ambient_strength * ambient_color * Diffuse * ambientOcclusion;
    
    vec4 diffuse = max(dot(Normal, -sunLightDir), 0.0) * Diffuse * (1.0 -shadow) * diffuse_strength;
    vec4 color = diffuse + ambient;
    float fog = fogColor(FragPos);
    if(enableFog){
        FragColor = mix(color, vec4(0.71,0.82,1.0,1.0), fog);
    }else{
        FragColor = color;
    }
}
