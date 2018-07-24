#version 330 core

in vec3 position;
in vec2 texCoord;

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vec4(position, 1.0);
    TexCoord = vec2(texCoord.x, 1.0f - texCoord.y);
}
