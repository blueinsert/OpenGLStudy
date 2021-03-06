#version 330 core

in vec3 Normal;
in vec3 WPos;

out vec4 FragColor;

uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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

void main()
{
    vec3 ambient = material.ambient * light.ambient;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - WPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = material.diffuse* diff * light.diffuse;

    vec3 viewDir = normalize(viewPos - WPos); 
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.specular;

    vec3 result = ambient+diffuse+specular;
    FragColor = vec4(result, 1.0);
}