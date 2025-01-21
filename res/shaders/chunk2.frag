#version 330 core


in vec4 out_color;
in vec3 out_normal;
in vec2 out_uv;

out vec4 FragColor;

uniform sampler2D out_texture;

void main()
{
    //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);

    vec3 sun_dir = normalize(vec3(0.1,0.8,0.1));
    float diffuse = max(dot(out_normal, sun_dir), 0.0);
    // //漫反射
    vec3 ambient_color = vec3(1.0,1.0,1.0);
    float ambient_strength = 0.8;
    vec3 ambient = ambient_strength * ambient_color;
    vec3 result = (ambient + diffuse);
    FragColor = texture(out_texture, out_uv) * vec4(result,1.0) * out_color;
   // FragColor = out_color * vec4(result,1.0);
}