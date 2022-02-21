#version 330 core

in vec3 Normal;
in vec3 WPos;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;
uniform sampler2D texture1;

uniform bool blinn;

vec4 Diffuse(vec2 uv){
    return texture(texture1, TexCoords);
}

vec4 Specular(vec2 uv){
    return texture(texture1, TexCoords);
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

#define NR_POINT_LIGHTS 1
uniform PointLight pointLights[NR_POINT_LIGHTS];


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 wPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - wPos);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    
    float spec = 0;
    if(blinn){
        vec3 halfwayDir = normalize(lightDir+viewDir);
        spec = pow(max(dot(halfwayDir, normal), 0.0), 16);
    }else{
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);
    }
    
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


void main()
{
    // 属性
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - WPos);

    vec3 result = vec3(0,0,0);
    // 第二阶段：点光源
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, WPos, viewDir);    

    FragColor = vec4(result, 1.0);
}