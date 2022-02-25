#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 WPos;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBN;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
	WPos = vec3(model * vec4(aPos, 1.0));
	mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
	//列主序，填充的依次第1,2,3列
    TBN = mat3(T, B, N);
	Normal = normalMatrix * aNormal;
	TexCoords = aTexCoords;
}