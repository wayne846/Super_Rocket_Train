#include "ParticleSystem.h"
#include "InstanceDrawer.h"

ParticleGenerator::Particle::Particle(glm::vec3 position, glm::vec3 velocity, int life) {
	this->position = position;
	this->velocity = velocity;
	this->lifeCount = life;
}

ParticleGenerator::ParticleGenerator(glm::vec3 position, int life, float generateRate, glm::vec3 particleColor, int particleLife, float gravity, Shader* shader) {
	this->position = position;
	this->lifeCount = life;
	this->generateRate = generateRate;
	this->particleColor = particleColor;
	this->particleLife = particleLife;
	this->gravity = gravity;
	this->shader = shader;
}

void ParticleGenerator::update() {
	for (auto p : particles) {
		//update velocity
		p.velocity += glm::vec3(0, -1, 0) * gravity;

		//update position
		p.position += p.velocity;

		p.lifeCount--;
	}
	particles.remove_if(isDead);
}

void ParticleGenerator::draw {
	
}

bool ParticleGenerator::isDead(const Particle& p) {
	if (p.lifeCount <= 0) {
		return true;
	}
	else {
		return false;
	}
}