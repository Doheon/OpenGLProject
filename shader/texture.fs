#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform int condition;


void main()
{
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.1);
}