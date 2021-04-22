#version 330 core

in vec3 lampcolor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(lampcolor, 1.0);
}