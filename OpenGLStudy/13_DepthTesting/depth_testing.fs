#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;
uniform sampler2D texture1;

void main()
{
    FragColor =texture(texture1,TexCoords);
}