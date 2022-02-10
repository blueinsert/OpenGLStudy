#ifndef PhongMaterials_H
#define PhongMaterials_H
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct PhongMaterial
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

const PhongMaterial Material_Emerald = {glm::vec3(0.0215f,0.1745f,0.0215f),glm::vec3(0.07568f,0.61424f,0.07568f),glm::vec3(0.633f,0.727811f,0.633f),0.6f};
const PhongMaterial Material_Jade = { glm::vec3(0.135,0.2225,0.1575),glm::vec3(0.54,0.89,0.63),glm::vec3(0.316228,0.316228,0.316228),0.1f };
const PhongMaterial Material_Gold = { glm::vec3(0.24725,0.1995,0.0745),glm::vec3(0.75164,0.60648,0.22648),glm::vec3(0.628281,0.555802,0.366065),0.4f };
const PhongMaterial Material_Silver = { glm::vec3(0.19225,0.19225,0.19225),glm::vec3(0.50754,0.50754,0.50754),glm::vec3(0.508273,0.508273,0.508273),0.4f };
const PhongMaterial Material_Pearl = { glm::vec3(0.25,0.20725,0.20725),glm::vec3(0.25,0.20725,0.20725),glm::vec3(0.25,0.20725,0.20725),0.088f };
const PhongMaterial Material_Ruby = { glm::vec3(0.1745,0.01175,0.01175),glm::vec3(0.1745,0.01175,0.01175),glm::vec3(0.1745,0.01175,0.01175),0.6f };
#endif