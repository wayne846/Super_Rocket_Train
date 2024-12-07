#include "ParticleSystem.h"
#include "InstanceDrawer.h"
#include "../MathHelper.h"

//---------------Particle System-----------------

ParticleSystem::ParticleSystem() {
	this->particleVAO = 0;
}

ParticleSystem::ParticleSystem(unsigned int particleVAO) {
	this->particleVAO = particleVAO;
}

void ParticleSystem::setParticleVAO(unsigned int p) {
	particleVAO = p;
}

ParticleGenerator& ParticleSystem::addParticleGenerator(Shader* shader) {
	particleGenerators.push_back(ParticleGenerator(shader, particleVAO));
	return particleGenerators.back();
}

ParticleGenerator* ParticleSystem::addParticleGenerator_pointer(Shader* shader) {
	particleGenerators.push_back(ParticleGenerator(shader, particleVAO));
	return &particleGenerators.back();
}

void ParticleSystem::deleteParticleGenerator(ParticleGenerator* generator) {
	for (auto p = particleGenerators.begin(); p != particleGenerators.end(); p++) {
		if (&(*p) == generator) {
			particleGenerators.erase(p);
			return;
		}
	}
}

//draw and update all particle generator
void ParticleSystem::update() {
	for (auto &p : particleGenerators) {
		p.update();
		if (p.lifeCount > 0) {
			p.lifeCount--;
		}
	}
	particleGenerators.remove_if(ParticleSystem::isDead);
}

void ParticleSystem::draw() {
	for (auto& p : particleGenerators) {
		p.draw();
	}
}

bool ParticleSystem::isDead(const ParticleGenerator& p) {
	if (p.lifeCount == 0 && p.isParticlesEmpty()) {
		return true;
	}
	else {
		return false;
	}
}


//---------------Particle Generator-----------------

ParticleGenerator::ParticleEntity::ParticleEntity() {
	this->attribute.position = glm::vec3(0, 0, 0);
	this->attribute.color = glm::vec3(0, 0, 0);
	this->attribute.size = 1;
	this->velocity = glm::vec3(0, 0, 0);
	this->lifeCount = 1;
}

ParticleGenerator::ParticleEntity::ParticleEntity(glm::vec3 position, glm::vec3 color, float size, glm::vec3 velocity, int life) {
	this->attribute.position = position;
	this->attribute.color = color;
	this->attribute.size = size;
	this->velocity = velocity;
	this->lifeCount = life;
}

ParticleGenerator::ParticleGenerator(Shader* shader, unsigned int particleVAO){
	this->shader = shader;
	this->particleVAO = particleVAO;

	this->position = glm::vec3(0, 0, 0);
	this->lifeCount = -1;
	this->generateRate = 5;
	this->particleGenerateCounter = 0;
	this->direction = glm::vec3(0, 1, 0);
	this->angle = 180;
	this->particleSize = 1;
	this->particleVelocity = 1;
	this->particleLife = 20;
	this->gravity = 0;
	this->friction = 1;
	this->color1 = glm::vec3(1, 1, 1);
	this->color2 = glm::vec3(1, 1, 1);
	this->color3 = glm::vec3(1, 1, 1);
	this->colorTransitionPoint = 0.5;
}

void ParticleGenerator::update() {
	//generate new particle
	if (lifeCount != 0) {
		particleGenerateCounter += generateRate;
		while (particleGenerateCounter >= 1) {
			ParticleEntity newParticle;
			newParticle.attribute.position = position;
			newParticle.attribute.color = MathHelper::gradientColor(color1, color2, color3, colorTransitionPoint, 0);
			newParticle.attribute.size = particleSize;
			newParticle.velocity = MathHelper::randomDirectionInCone(direction, angle);
			newParticle.lifeCount = particleLife;
			particles.push_back(newParticle);

			particleGenerateCounter -= 1.0f;
		}
	}
	

	//update all particle
	for (auto& p : particles) {
		//update velocity
		p.velocity += glm::vec3(0, -1, 0) * gravity;
		p.velocity = p.velocity * friction;

		//update position
		p.attribute.position += p.velocity * particleVelocity;

		p.attribute.color = MathHelper::gradientColor(color1, color2, color3, colorTransitionPoint, (float)(particleLife - p.lifeCount) / particleLife);
		p.lifeCount--;
	}
	particles.remove_if(ParticleGenerator::isDead);
}

void ParticleGenerator::draw() {
	for (auto& p : particles) {
		instanceDrawer.addParticleAttribute(p.attribute);
	}
	instanceDrawer.drawParticleByInstance(shader, particleVAO);
}

bool ParticleGenerator::isParticlesEmpty() const {
	return particles.empty();
}

bool ParticleGenerator::isDead(const ParticleEntity& p) {
	if (p.lifeCount <= 0) {
		return true;
	}
	else {
		return false;
	}
}

void ParticleGenerator::setPosition(glm::vec3 pos) {
	position = pos;
}

void ParticleGenerator::setLife(int life) {
	lifeCount = life;
}

void ParticleGenerator::setGenerateRate(float rate) {
	generateRate = rate;
}

void ParticleGenerator::setDirection(glm::vec3 dir) {
	direction = dir;
}

void ParticleGenerator::setAngle(float a) {
	angle = a;
}

void ParticleGenerator::setParticleSize(float size) {
	particleSize = size;
}

void ParticleGenerator::setParticleVelocity(float velocity) {
	particleVelocity = velocity;
}

void ParticleGenerator::setParticleLife(float life) {
	particleLife = life;
}

void ParticleGenerator::setGravity(float g) {
	gravity = g;
}

void ParticleGenerator::setFriction(float f) {
	friction = f;
}

void ParticleGenerator::setColor(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, float transitionPoint) {
	color1 = c1;
	color2 = c2;
	color3 = c3;
	colorTransitionPoint = transitionPoint;
}