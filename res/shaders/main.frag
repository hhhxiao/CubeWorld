#version 330 core
out vec4 FragColor;
in vec3 outColor;
in vec2 outTexCoord;
in vec3 outNormal;

in vec3 FragPos;

uniform sampler2D outTexture;



void main()
{
    // FragColor = texture(outTexture, outTexCoord);
    

    vec3 lightDir = normalize(vec3(0.1,0.8,0.1));
    float diffuse = max(dot(outNormal, lightDir), 0.0);
    //漫反射
    vec3 ambientLightColor = vec3(1.0,1.0,1.0);
    float ambientStrength = 0.8;
    vec3 ambient = ambientStrength * ambientLightColor;

    vec3 result = (ambient + diffuse);

    FragColor = texture(outTexture, outTexCoord) * vec4(result,1.0);

}