#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    //经度: [-pi,pi]
    //维度：[-pi/2,pi/2]
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;//转到值域[-0.5,0.5]
    uv += 0.5;//转到值域[0,1]
    return uv;
}

void main()
{		
    vec2 uv = SampleSphericalMap(normalize(WorldPos));
    vec3 color = texture(equirectangularMap, uv).rgb;
    
    FragColor = vec4(color, 1.0);
}
