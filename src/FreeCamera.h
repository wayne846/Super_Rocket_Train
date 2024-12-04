#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Fl_Gl_Window;

class FreeCamera{
private:
	float Speed = 5.0f;
	float MouseSensitivity = 40.0f;

	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 up;

	float lastX;
	float lastY;
	
	float Yaw;
	float Pitch;

	enum class Mode {
		None,
		Pan,
		Rotate
	};
	Mode mode;

	Fl_Gl_Window* window;

	void reset();
	void getMouseNDC(float& x, float& y);

public:
	const float FOV_ = 60.0f;
	const float NEAR_ = 0.1f;
	const float FAR_ = 1000.f;

	FreeCamera();
	FreeCamera(Fl_Gl_Window *window);

	int handle(int e);

	void setWindow(Fl_Gl_Window* w);

	glm::vec3 getPosition();
	glm::vec3 getDirection();
	glm::vec3 getUp();
};

