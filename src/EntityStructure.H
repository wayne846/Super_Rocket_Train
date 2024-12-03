#pragma once
#include "Utilities/Pnt3f.H"
#include <string>
struct Entity {
	Entity(Pnt3f pos, Pnt3f orient, Pnt3f up) {
		this->pos = pos;
		this->front = orient;
		this->up = up;
		this->state = 0;
		this->speed = 0;
	}
	Pnt3f pos;
	Pnt3f front;
	Pnt3f up;
	int state;
	int speed;
};