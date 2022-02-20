#version 330 core

in vec3 Normal;
in vec3 WPos;
in vec2 TexCoords;
in vec3 Tangent;
in vec3 Bitangent;

out vec4 FragColor;

uniform vec3 viewPos;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
	sampler2D texture_normal1;
    float shininess;
}; 

uniform Material material;

vec4 Diffuse(vec2 uv){
    return texture(material.texture_diffuse1, TexCoords);
}

vec4 Specular(vec2 uv){
	return texture(material.texture_specular1, TexCoords);
}

vec4 NormalTexture(vec2 uv){
	return texture(material.texture_normal1, TexCoords);
}

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight;

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutoff_inner;//Ω«∂»cos÷µ
    float cutoff_outter;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

uniform SpotLight spotLight;

void main()
{
    FragColor = vec4(Diffuse(TexCoords).xyz, 1.0);
	//FragColor = vec4(viewDir, 1.0);
}