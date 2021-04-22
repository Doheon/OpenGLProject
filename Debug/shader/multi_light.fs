#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct dirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct pointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct spotLight {
    vec3 position;
    vec3 direction;
    float cutoff;
    float outercutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 realPos;

#define pointnum 3
uniform Material material;
uniform dirLight direct;
uniform pointLight point[pointnum];
uniform spotLight spot;

uniform vec3 lightPos;
uniform vec3 viewPos;

vec3 CalcdirLight(dirLight light, vec3 normal, vec3 viewDir);
vec3 CalcpointLight(pointLight light, vec3 normal, vec3 viewDir);
vec3 CalcspotLight(spotLight light, vec3 normal, vec3 viewDir);

void main()
{
     vec3 norm = normalize(Normal);
     vec3 viewDir = normalize(viewPos - realPos);

     vec3 result = vec3(0, 0, 0);
     result += CalcdirLight(direct, norm, viewDir);
     for(int i=0; i<pointnum; i++){
          result += CalcpointLight(point[i], norm, viewDir);
     }
     result += CalcspotLight(spot, norm, viewDir);

     FragColor = vec4(result, 1.0);
}

vec3 CalcdirLight(dirLight light, vec3 normal, vec3 viewDir)
{
     vec3 lightDir = normalize(-light.direction);
     float diff = max(dot(normal, lightDir), 0.0);
     vec3 reflectDir = reflect(-lightDir, normal);
     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

     vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
     vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
     vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
     return (ambient + diffuse + specular);
}

vec3 CalcpointLight(pointLight light, vec3 normal, vec3 viewDir)
{
     vec3 lightDir = normalize(light.position - realPos);
     float diff = max(dot(normal, lightDir), 0.0);
     vec3 reflectDir = reflect(-lightDir, normal);
     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

     float distance = length(light.position - realPos);
     float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

     vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
     vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
     vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
     return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcspotLight(spotLight light, vec3 normal, vec3 viewDir)
{
     vec3 lightDir = normalize(light.position - realPos);
     float diff = max(dot(normal, lightDir), 0.0);
     vec3 reflectDir = reflect(-lightDir, normal);
     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

     float distance = length(light.position - realPos);
     float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

     float theta = dot(lightDir, normalize(-light.direction));
     float epsilon = light.cutoff - light.outercutoff;
     float intensity = clamp((theta - light.outercutoff) / epsilon, 0.0, 1.0);   

     vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
     vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
     vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));

     return (ambient + diffuse + specular) * attenuation * intensity;

}









