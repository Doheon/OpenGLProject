#version 330 core
struct Material {
    vec3 diffuse;
    vec3 specular;
    float alpha;
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

out vec4 FragColor;

in vec3 Normal;
in vec3 realPos;

#define pointnum 3
uniform Material material;
uniform dirLight direct;
uniform pointLight point[pointnum];
uniform vec3 viewPos;
uniform int grid;
uniform float shadow;

vec3 CalcdirLight(dirLight light, vec3 normal, vec3 viewDir);
vec3 CalcpointLight(pointLight light, vec3 normal, vec3 viewDir);


void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - realPos);

    vec3 result = vec3(0, 0, 0);
    for(int i=0; i<pointnum; i++){
        result += CalcpointLight(point[i], norm, viewDir);
    }
    result += CalcdirLight(direct, norm, viewDir);
    FragColor = vec4(shadow, shadow, shadow, 1) * vec4(result, material.alpha);
}

vec3 CalcdirLight(dirLight light, vec3 normal, vec3 viewDir)
{
     vec3 lightDir = normalize(-light.direction);
     float diff = max(dot(normal, lightDir), 0.0);
     vec3 reflectDir = reflect(-lightDir, normal);
     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

     vec3 ambient = light.ambient * material.diffuse;
     vec3 diffuse = light.diffuse * diff * material.diffuse;
     vec3 specular = light.specular * spec * material.specular;
     return (ambient + diffuse + specular);
}



vec3 CalcpointLight(pointLight light, vec3 normal, vec3 viewDir)
{
     vec3 lightDir = normalize(light.position - realPos);
     float diff = max(dot(normal, lightDir), 0.0);
     vec3 reflectDir = reflect(-lightDir, normal);

     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

     float distance = length(light.position - realPos) / 15.0;
     float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

     vec3 ambient = light.ambient * material.diffuse;
     vec3 diffuse = light.diffuse * diff * material.diffuse;
     vec3 specular = light.specular * spec * material.specular;
     return (ambient + diffuse + specular) * attenuation;
}