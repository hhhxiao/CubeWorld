#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform float scale = 0.3; // scale should be set to 1/n
uniform float aspectRatio; // aspect ratio of the screen (width / height)

void main()
{
    vec3 scaledPosition = aPosition;
    scaledPosition.x *= scale;
    scaledPosition.y *= scale;
    scaledPosition.x -= (1.0 - scale);
    scaledPosition.y -= (1.0 - scale);
    gl_Position = vec4(scaledPosition, 1.0);
    TexCoords = aTexCoords;
}