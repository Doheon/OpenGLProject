#version 330 core

out vec4 FragColor;
in vec3 scolor;

uniform float grid;

void main()
{
    FragColor = grid * vec4(scolor + 0.3, 1.0);
}