#pragma once
#include <vector>
#include <string>
#include "RanderStructure.h"
#include "Shader.h"
#include <glm/glm.hpp>

class Instance {
private:
	struct ObjIns
	{
		ObjIns(std::string name, glm::mat4 modelMatrix, Material material) {
			this->name = name;
			this->modelMatrices.push_back(modelMatrix);
			this->material = material;
		}
		std::string name;
		std::vector<glm::mat4> modelMatrices;
		Material material;
	};
	std::vector<ObjIns> objins;
	GLuint instanceVBO;
public:
	Instance() {
		instanceVBO = 0;
	}
	void addInstance(std::string name, glm::mat4 modelMatrix, Material &material);
	void drawByInstance(Shader* shader, Object &object);
};