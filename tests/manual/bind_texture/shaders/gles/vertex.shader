attribute vec3 position;
attribute vec3 color;
attribute vec2 texCoord;

varying vec3 ourColor;
varying vec2 TexCoord;

void main() {
    ourColor = color;
    TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
    gl_Position = vec4(position, 1.0);
}
