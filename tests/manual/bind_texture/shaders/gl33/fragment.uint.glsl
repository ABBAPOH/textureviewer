#version 330 core

in vec2 TexCoord;

out vec4 color;

uniform usampler2D ourTexture1;

void main()
{
    color = vec4(texture(ourTexture1, TexCoord) / 255.0);
}
