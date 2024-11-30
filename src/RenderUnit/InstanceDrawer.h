#pragma once
#include <vector>
#include <string>
#include "RanderStructure.h"
#include "Shader.h"
#include <glm/glm.hpp>

class InstanceDrawer {
private:
	std::vector<glm::mat4> modelMatrices;
	std::vector<glm::mat4> normalMatrices;
	Material material;

	GLuint instanceVBO[2];
public:
	InstanceDrawer();
	InstanceDrawer(const Material& m);

	void addModelMatrix(glm::mat4 modelMatrix);
	void setMaterial(const Material& m);
	void drawByInstance(Shader* shader, Object &object);
};