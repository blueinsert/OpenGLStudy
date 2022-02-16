#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;
uniform sampler2D texture1;

float near = 0.1; 
float far  = 100.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main()
{
    FragColor =texture(texture1,TexCoords);
	          
    //float z = LinearizeDepth(gl_FragCoord.z); 
	//float depth = clamp((z-near)/ (far-near),0.0,1.0); // 为了演示除以 far
    //FragColor = vec4(vec3(depth), 1.0);
}