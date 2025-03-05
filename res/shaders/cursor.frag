#version 330 core
vec2 TexCoords;
out vec4 FragColor;
uniform sampler2D gAlbedo;
uniform int srcWidth;
uniform int srcHeight;
uniform int LINE_WIDTH =  3;
uniform int TOTAL_WIDTH = 15; 
bool check() {
    int wc = srcWidth / 2;
    int hc = srcHeight / 2;
    float dx = abs(wc -  gl_FragCoord.x);
    float dy = abs(hc - gl_FragCoord.y);
    return dx <= TOTAL_WIDTH && dy <= TOTAL_WIDTH && (dx <= LINE_WIDTH || dy <= LINE_WIDTH);
}

void main(){
   if(!check())discard;
    FragColor = vec4(0.9, 0.9, 0.9, 0.01);
   // FragColor = texture(gAlbedo, TexCoords);
}
