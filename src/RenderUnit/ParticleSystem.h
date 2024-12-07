#pragma once
#include <glm/glm.hpp>
#include <list>
#include "Shader.h"
#include "RenderStructure.h"
#include "InstanceDrawer.h"

class ParticleGenerator;

class ParticleSystem{
private:
	std::list<ParticleGenerator> particleGenerators;
	unsigned int particleVAO;

	static bool isDead(const ParticleGenerator& p);

public:

	ParticleSystem();
	ParticleSystem(unsigned int particleVAO);

	void setParticleVAO(unsigned int p);

	//return the reference of added particle generator, you can use seter function to set the attribute
	ParticleGenerator& addParticleGenerator(Shader* shader);

	//return the pointer of added particle generator, you can use seter function to set the attribute
	ParticleGenerator* addParticleGenerator_pointer(Shader* shader);
	void deleteParticleGenerator(ParticleGenerator* generator);

	void update();
	void draw();
};

class ParticleGenerator {
private:

	class ParticleEntity {
	public:
		Particle attribute; //pos, color, size

		glm::vec3 velocity;
		int lifeCount;

		ParticleEntity();
		ParticleEntity(glm::vec3 position, glm::vec3 color, float size, glm::vec3 velocity, int life);
	};

	// generator self attribute
	glm::vec3 position;
	float particleGenerateCounter;
	
	// particle attribute
	float generateRate;
	glm::vec3 direction;
	float angle;
	float particleSize;
	float particleVelocity;
	int particleLife;
	float gravity;
	float friction;
	glm::vec3 color1;
	glm::vec3 color2;
	glm::vec3 color3;
	float colorTransitionPoint;

	// for render
	Shader* shader;
	InstanceDrawer instanceDrawer;
	unsigned int particleVAO;

	std::list<ParticleEntity> particles;

	static bool isDead(const ParticleEntity& p);

public:

	int lifeCount;
	
	ParticleGenerator(Shader* shader, unsigned int particleVAO);

	void update();
	void draw();

	bool isParticlesEmpty() const;

	void setPosition(glm::vec3 pos);
	void setLife(int life);
	void setGenerateRate(float rate);
	void setDirection(glm::vec3 dir);
	void setAngle(float a);
	void setParticleSize(float size);
	void setParticleVelocity(float velocity);
	void setParticleLife(float life);
	void setGravity(float g);
	void setFriction(float f);
	void setColor(glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, float transitionPoint);
};

