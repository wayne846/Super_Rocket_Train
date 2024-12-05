#pragma once
#include "Utilities/Pnt3f.H"
#include <string>
class Entity {
public:
	Entity(Pnt3f pos, Pnt3f front, Pnt3f up) {
		this->pos = pos;
		this->front = front;
		this->up = up;
		this->state = 0;
	}
	Pnt3f pos;
	Pnt3f front;
	Pnt3f up;
	int state;
};

class Rocket :public Entity {
public:
Rocket(Pnt3f pos, Pnt3f front, Pnt3f up): Entity(pos, front, up) {
	lastPos = pos;
}
	Pnt3f thrusterVelocity;
	Pnt3f gravityVelocity;
	Pnt3f lastPos;
	void advance() {
		pos = pos + thrusterVelocity + gravityVelocity;
		if(thrusterVelocity.len2()<100)
			thrusterVelocity = thrusterVelocity * 1.05;	// accelerate
		gravityVelocity.y -= 0.2;	// g
		front = thrusterVelocity + gravityVelocity;
		Pnt3f right = front * up;
		up = right * front;
		// if you don't normalize it, something cool will happend
		front.normalize();
		up.normalize();
	}
};

class PhysicalEntity :public Entity {
public:
	PhysicalEntity (Pnt3f pos, Pnt3f front, Pnt3f up) : Entity(pos, front, up) {
	}
	Pnt3f velocity;
	Pnt3f angularVelocity;
	void advance() {
		pos = pos + velocity;
		velocity = velocity * 0.995;
		velocity.y -= 0.1;	// g
		// Todo: rotate it
	}
};