#pragma once
#include <glm/glm.hpp>
#include <list>
#include "Shader.h"

class ParticleGenerator;

class ParticleSystem{
private:

public:

	std::list<ParticleGenerator> particleGenerators;

	void addParticleGenerator();
};

class ParticleGenerator {
private:
	class Particle {
	public:
		glm::vec3 position;
		glm::vec3 velocity;
		int lifeCount;

		Particle(glm::vec3 position, glm::vec3 velocity, int life);
	};

	static bool isDead(const Particle& p);

public:
	glm::vec3 position;
	int lifeCount;

	float generateRate;
	glm::vec3 particleColor;
	int particleLife;
	float particleSize;
	float gravity;

	Shader* shader;

	std::list<Particle> particles;

	ParticleGenerator(glm::vec3 position, int life, float generateRate, glm::vec3 particleColor, int particleLife, float gravity, Shader* shader);
	void update();
	//void draw();
};

