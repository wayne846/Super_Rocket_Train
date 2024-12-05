#pragma once
#include <vector>
#include <string>
#include "RenderStructure.h"
#include "Shader.h"
#include <glm/glm.hpp>

class InstanceDrawer {
private:
	std::vector<glm::mat4> modelMatrices;
	std::vector<glm::mat4> normalMatrices;
	Material material;
	unsigned int textureId=-1;

	GLuint instanceVBO[2];
public:
	InstanceDrawer();
	InstanceDrawer(const Material& m);
	~InstanceDrawer();

	void addModelMatrix(glm::mat4 modelMatrix);
	void setMaterial(const Material& m);
	void setTexture(unsigned int id);
	void drawByInstance(Shader* shader, Object &object);
	void drawParticleByInstance(Shader* shader, glm::vec3 color, float size, std::vector<glm::vec3> pos);
};