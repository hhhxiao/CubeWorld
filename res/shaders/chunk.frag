#version 420 core


in vec4 out_color;
in vec3 out_normal;
in vec2 out_uv;
in vec3 FragPos;
in vec4 FragPosLightSpace;

out vec4 FragColor;

layout(binding=0) uniform sampler2D out_texture;
layout(binding=1) uniform sampler2D shadowMap;


uniform vec3 world_camera;
//fog
uniform bool enableFog;
uniform float fogNear;
uniform float fogFar;

float fogColor(){
    float dist = distance(world_camera, FragPos);
    float fact = 1.0 * (dist - fogNear)/(fogFar - fogNear);
    return clamp(fact, 0.0, 1.0);
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    // Check if projCoords are outside the shadow map
    if(projCoords.z > 1.0)
        return 0.0;

    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;

    // Bias to prevent shadow acne
    float bias = max(0.005 * (1.0 - dot(out_normal, vec3(0.1, 0.8, 0.1))), 0.005);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main()
{

    vec3 sun_dir = normalize(vec3(0.1,0.8,0.1));
    float diffuse = max(dot(out_normal, sun_dir), 0.0);
    // //漫反射
    vec3 ambient_color = vec3(1.0,1.0,1.0);
    float ambient_strength = 0.6;
    vec3 ambient = ambient_strength * ambient_color;

    float shadow = ShadowCalculation(FragPosLightSpace);

    vec3 result = (ambient +  (1.0 - shadow) * diffuse * 0.9) ;
    vec4 color = texture(out_texture, out_uv) * vec4(result,1.0) * out_color;

    float fog = fogColor();
    if(enableFog){
        FragColor = mix(color, vec4(0.71,0.82,1.0,1.0), fog);
    }else{
        FragColor = color;
    }
}
