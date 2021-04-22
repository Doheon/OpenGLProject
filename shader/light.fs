#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 
uniform Material material;


struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;


out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 realPos;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
     float dis = distance(lightPos, realPos);
     dis = 1;

     vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));

     vec3 norm = normalize(Normal);
     vec3 lightDir = normalize(lightPos - realPos);
     float diff = max(dot(norm, lightDir), 0.0);
     vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

     vec3 viewDir = normalize(viewPos - realPos);
     vec3 reflectDir = reflect(-lightDir, norm);
     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
     vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

     vec3 result = (ambient+diffuse+specular)/dis;
     FragColor = vec4(result, 1.0);
}