#pragma once
#include "InstanceDrawer.h"
#include <glad/glad.h>

InstanceDrawer::InstanceDrawer() {
	this->instanceVBO[0] = 0;
	this->instanceVBO[1] = 0;
	this->textureId = -1;
}

InstanceDrawer::~InstanceDrawer() {
	if (instanceVBO[0] != 0 || instanceVBO[1] != 0) {
		glDeleteBuffers(3, this->instanceVBO);
	}
}

InstanceDrawer::InstanceDrawer(const Material& m) {
	this->instanceVBO[0] = 0;
	this->instanceVBO[1] = 0;

	this->material = m;
}

void InstanceDrawer::addModelMatrix(glm::mat4 modelMatrix)
{
	modelMatrices.push_back(modelMatrix);
	normalMatrices.push_back(glm::transpose(glm::inverse(modelMatrix)));
}

void InstanceDrawer::setMaterial(const Material& m) {
	material = m;
}

void InstanceDrawer::setTexture(unsigned int id)
{
	textureId = id;
}

//draw the object by all model matrix, the model ande normal matrix will be clear after drawed
void InstanceDrawer::drawByInstance(Shader* shader, Object &object)
{
	if (instanceVBO[0] == 0 && instanceVBO[1] == 0) {
		glGenBuffers(2, this->instanceVBO);
	}
	glBindVertexArray(object.VAO);
	shader->use();

	// material properties
	shader->setVec3("material.ambient", material.ambient);
	shader->setVec3("material.diffuse", material.diffuse);
	shader->setVec3("material.specular", material.specular);
	shader->setFloat("material.shininess", material.shininess);

	// set texture
	if (this->textureId != -1) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
		shader->setBool("useImage", true);
		shader->setInt("imageTexture", 0);
	}
	else {
		shader->setBool("useImage", false);
	}	

	//-------------------
	// set instance VBO
	//-------------------
	// set model matrix
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), modelMatrices.data(), GL_STATIC_DRAW);
	for (int j = 0; j < 4; j++) {
		int location = 3 + j;
		glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(j * sizeof(glm::vec4)));
		glEnableVertexAttribArray(location);
		glVertexAttribDivisor(location, 1);
	}

	// set normal matrix
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, normalMatrices.size() * sizeof(glm::mat4), normalMatrices.data(), GL_STATIC_DRAW);
	for (int j = 0; j < 4; j++) {
		int location = 7 + j;
		glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(j * sizeof(glm::vec4)));
		glEnableVertexAttribArray(location);
		glVertexAttribDivisor(location, 1);
	}

	glDrawElementsInstanced(GL_TRIANGLES, object.element_amount, GL_UNSIGNED_INT, 0, modelMatrices.size());
	//unbind VAO
	glBindVertexArray(0);
	//unbind Texture
	glBindTexture(GL_TEXTURE_2D, 0);
	//unbind shader(switch to fixed pipeline)
	glUseProgram(0);

	modelMatrices.clear();
	normalMatrices.clear();
}

void InstanceDrawer::addParticleAttribute(Particle attribute) {
	particlAttributes.push_back(attribute);
}

void InstanceDrawer::drawParticleByInstance(Shader* shader, const unsigned int particleVAO) {
	if (instanceVBO[0] == 0) {
		glGenBuffers(1, this->instanceVBO);
	}
	glBindVertexArray(particleVAO);
	shader->use();

	//-------------------
	// set instance VBO
	//-------------------
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, particlAttributes.size() * sizeof(Particle), particlAttributes.data(), GL_DYNAMIC_DRAW);

	// 配置粒子屬性：位置
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, position));
	glEnableVertexAttribArray(0);
	glVertexAttribDivisor(0, 1); // 每個實例使用一個位置數據

	// 配置粒子屬性：顏色
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
	glEnableVertexAttribArray(1);
	glVertexAttribDivisor(1, 1);

	// 配置粒子屬性：大小
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, size));
	glEnableVertexAttribArray(2);
	glVertexAttribDivisor(2, 1);

	// 配置粒子屬性：速度
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, velocity));
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ONE);
	glDrawArraysInstanced(GL_POINTS, 0, 1, particlAttributes.size());
	//glDisable(GL_BLEND);

	//unbind VAO
	glBindVertexArray(0);

	//unbind shader(switch to fixed pipeline)
	glUseProgram(0);

	particlAttributes.clear();
}

