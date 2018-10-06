#version 330 core

in vec2 TexCoord;

out vec4 color;

uniform isampler2D ourTexture1;

void main()
{
    color = vec4(texture(ourTexture1, TexCoord) / 127.0);
}
