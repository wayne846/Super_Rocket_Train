#pragma once
#include <vector>
#include <string>
#include "RenderStructure.h"
#include "Shader.h"
#include <glm/glm.hpp>

class InstanceDrawer {
private:
	//for object
	std::vector<glm::mat4> modelMatrices;
	std::vector<glm::mat4> normalMatrices;
	Material material;
	unsigned int textureId=-1;

	GLuint instanceVBO[2];


	//for particle
	std::vector<Particle> particlAttributes;

public:
	InstanceDrawer();
	InstanceDrawer(const Material& m);
	~InstanceDrawer();

	void addModelMatrix(glm::mat4 modelMatrix);
	void setMaterial(const Material& m);
	void setTexture(unsigned int id);
	void drawByInstance(Shader* shader, Object &object);

	void addParticleAttribute(Particle attribute);
	void drawParticleByInstance(Shader* shader, const unsigned int particleVAO);
};