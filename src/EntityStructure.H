#pragma once
#include "Utilities/Pnt3f.H"
#include <string>
class Entity {
public:
	Entity(Pnt3f pos, Pnt3f orient, Pnt3f up) {
		this->pos = pos;
		this->lastPos = pos;
		this->front = orient;
		this->up = up;
		this->state = 0;
	}
	Pnt3f pos;
	Pnt3f lastPos;
	Pnt3f front;
	Pnt3f up;
	int state;
	Pnt3f velocity;
	Pnt3f angularVelocity;

	void advance() {
		pos = pos + velocity;
		velocity = velocity * 0.995;
		velocity.y -= 0.1;	// g
		// Todo rotate it
	}
};