varying vec2 TexCoord;

uniform usampler2D ourTexture1;

void main() {
   gl_FragColor = texture2D(ourTexture1, TexCoord);
}
