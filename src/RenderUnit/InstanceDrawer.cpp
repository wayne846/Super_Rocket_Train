#pragma once
#include "InstanceDrawer.h"
#include <glad/glad.h>

InstanceDrawer::InstanceDrawer() {
	this->instanceVBO[0] = 0;
	this->instanceVBO[1] = 0;
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

//draw the object by all model matrix, the model ande normal matrix will be clear after drawed
void InstanceDrawer::drawByInstance(Shader* shader, Object &object)
{
	if (instanceVBO[0] == 0 && instanceVBO[1] == 0) {
		glGenBuffers(2, instanceVBO);

	}
	glBindVertexArray(object.VAO);
	shader->use();

	// material properties
	shader->setVec3("material.ambient", material.ambient);
	shader->setVec3("material.diffuse", material.diffuse);
	shader->setVec3("material.specular", material.specular);
	shader->setFloat("material.shininess", material.shininess);

	//-------------------
	// set instance VBO
	//-------------------
	// set model matrix
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), modelMatrices.data(), GL_STATIC_DRAW);
	for (int j = 0; j < 4; j++) {
		int location = 2 + j;
		glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(j * sizeof(glm::vec4)));
		glEnableVertexAttribArray(location);
		glVertexAttribDivisor(location, 1);
	}

	// set normal matrix
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, normalMatrices.size() * sizeof(glm::mat4), normalMatrices.data(), GL_STATIC_DRAW);
	for (int j = 0; j < 4; j++) {
		int location = 6 + j;
		glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(j * sizeof(glm::vec4)));
		glEnableVertexAttribArray(location);
		glVertexAttribDivisor(location, 1);
	}

	glDrawElementsInstanced(GL_TRIANGLES, object.element_amount, GL_UNSIGNED_INT, 0, modelMatrices.size());
	//unbind VAO
	glBindVertexArray(0);
	//unbind shader(switch to fixed pipeline)
	glUseProgram(0);

	modelMatrices.clear();
	normalMatrices.clear();
}

