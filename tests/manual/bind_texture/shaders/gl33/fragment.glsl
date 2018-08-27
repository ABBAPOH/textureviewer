#version 330 core

in vec2 TexCoord;

out vec4 color;

uniform usampler2D ourTexture1;

void main()
{
    uvec4 tmp = texture(ourTexture1, TexCoord);
    color = vec4(tmp.x / 127.0f, tmp.y / 255.0f, tmp.z / 255.0f, tmp.w / 255.0f);
//    color = vec4(1.0f, tmp.g / 255.0f, tmp.b / 255.0f, tmp.a / 255.0f);
}
