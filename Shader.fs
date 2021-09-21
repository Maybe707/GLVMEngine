#version 410 core
out vec4 FragColor;

in vec2 TexCoord;
// Texture sampler
uniform sampler2D tex;

void main()
{
    FragColor = texture(tex, TexCoord);
}
