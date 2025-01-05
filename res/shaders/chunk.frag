#version 330 core
out vec4 FragColor;
in vec4 outColor;
in vec2 outTexCoord;
in vec3 outNormal;

in vec3 FragPos;

uniform sampler2D outTexture;



void main()
{
    vec4 texColor = texture(outTexture, outTexCoord) * outColor;
    // if(texColor.a < 0.5)
    //     discard;

    vec3 lightDir = normalize(vec3(0.1,0.8,0.1));
    float diffuse = max(dot(outNormal, lightDir), 0.0);
    //漫反射
    vec3 ambientLightColor = vec3(1.0,1.0,1.0);
    float ambientStrength = 0.6;
    vec3 ambient = ambientStrength * ambientLightColor;

    vec3 result = (ambient + diffuse);

    FragColor =  texColor  * vec4(result,1.0);
}