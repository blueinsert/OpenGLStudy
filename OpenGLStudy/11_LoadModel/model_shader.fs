#version 330 core

in vec3 Normal;
in vec3 WPos;
in vec2 TexCoords;
in mat3 TBN;

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
    float cutoff_inner;//角度cos值
    float cutoff_outter;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

uniform SpotLight spotLight;

uniform bool useNormalMapping;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 合并结果
    vec3 ambient  = light.ambient  * vec3(Diffuse(TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(Diffuse(TexCoords));
    vec3 specular = light.specular * spec * vec3(Specular(TexCoords));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 wPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - wPos);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 衰减
    float distance    = length(light.position - wPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                 light.quadratic * (distance * distance));    
    // 合并结果
    vec3 ambient  = light.ambient  * vec3(Diffuse(TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(Diffuse(TexCoords));
    vec3 specular = light.specular * spec * vec3(Specular(TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight spotLight, vec3 normal, vec3 wPos, vec3 viewDir){
    vec3 lightDir = normalize(spotLight.position-wPos);
    float theta = dot(lightDir,normalize(-spotLight.direction));
    float intensity = clamp((theta-spotLight.cutoff_outter)/(spotLight.cutoff_inner-spotLight.cutoff_outter),0.0,1.0);

    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 衰减
    float distance = length(spotLight.position - wPos);
    float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance));    
    // 合并结果
    vec3 ambient  = spotLight.ambient  * vec3(Diffuse(TexCoords));
    vec3 diffuse  = spotLight.diffuse  * diff * vec3(Diffuse(TexCoords));
    vec3 specular = spotLight.specular * spec * vec3(Specular(TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular)*intensity;
}

void main()
{
    // 属性
    vec3 norm = normalize(Normal);
    if(useNormalMapping){
        norm = NormalTexture(TexCoords).xyz;
        // transform normal vector to range [-1,1]
        norm = normalize(norm * 2.0 - 1.0);  // this normal is in tangent space
        norm = TBN * norm;
    }
    vec3 viewDir = normalize(viewPos - WPos);

    // 第一阶段：定向光照
    vec3 result = vec3(0,0,0);
    result += CalcDirLight(dirLight, norm, viewDir);
    // 第二阶段：点光源
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, WPos, viewDir);    
    // 第三阶段：聚光
    result += CalcSpotLight(spotLight, norm, WPos, viewDir);    

    FragColor = vec4(result, 1.0);
	//FragColor = vec4(viewDir, 1.0);
}