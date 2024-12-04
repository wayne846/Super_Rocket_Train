#include "FreeCamera.h"

//I copy these from ArcBallCam.cpp
// the FlTk headers have lots of warnings - these are bad, but there's not
// much we can do about them
#pragma warning(push)
#pragma warning(disable:4311)		// convert void* to long
#pragma warning(disable:4312)		// convert long to void*
#include <FL/Fl_Gl_Window.h>
#include <Fl/Fl.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <Fl/Fl_Double_Window.h>
#pragma warning(pop)

#include "Utilities/3DUtils.h"

FreeCamera::FreeCamera() {
	this->window = 0;
	reset();
}

FreeCamera::FreeCamera(Fl_Gl_Window* window) {
	this->window = window;
	reset();
}

void FreeCamera::reset() {
	position = glm::vec3(200, 100, 0);
	direction = glm::vec3(-1, 0, 0);
	up = glm::vec3(0, 1, 0);

	lastX = 0;
	lastY = 0;
	Yaw = 180;
	Pitch = 0;

	mode = Mode::None;
}

int FreeCamera::handle(int e) {
	switch (e) {
		case FL_PUSH:
			// right mouse button down
			if (Fl::event_button() == FL_RIGHT_MOUSE) {
				// double click? do a reset
				if (Fl::event_clicks()) {
					reset();
					window->damage(1);
					return 1;
				}

				// Get the mouse position
				getMouseNDC(lastX, lastY);

				// Tell window to refresh
				window->damage(1);

				mode = Mode::Rotate;
				return 1;
			}
			else if (Fl::event_button() == FL_MIDDLE_MOUSE) {
				// Get the mouse position
				getMouseNDC(lastX, lastY);

				// Tell window to refresh
				window->damage(1);

				mode = Mode::Pan;
				return 1;
			}
			break;

		case FL_RELEASE:
			if (mode != Mode::None) {
				mode = Mode::None;
			}
			break;

		case FL_KEYBOARD:
			if (mode != Mode::None) {
				glm::vec3 right = glm::cross(direction, up);
				glm::vec3 moveDir = glm::vec3(0, 0, 0);
				if (Fl::event_key((int)'w') || Fl::event_key((int)'W')) {
					moveDir += direction;
				}
				if (Fl::event_key((int)'a') || Fl::event_key((int)'A')) {
					moveDir -= right;
				}
				if (Fl::event_key((int)'s') || Fl::event_key((int)'S')) {
					moveDir -= direction;
				}
				if (Fl::event_key((int)'d') || Fl::event_key((int)'D')) {
					moveDir += right;
				}
				position += moveDir * Speed;
			}
			

		case FL_DRAG: // if the user drags the mouse
			if (mode == Mode::Rotate) { // we're taking the drags
				float currentX, currentY;
				getMouseNDC(currentX, currentY);
				Yaw += -(currentX - lastX) * MouseSensitivity;
				Pitch += -(currentY - lastY) * MouseSensitivity;
				// make sure that when pitch is out of bounds, screen doesn't get flipped
				if (Pitch > 89.0f)
					Pitch = 89.0f;
				if (Pitch < -89.0f)
					Pitch = -89.0f;

				lastX = currentX;
				lastY = currentY;
				
				// calculate the new Front vector
				glm::vec3 front;
				front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
				front.y = sin(glm::radians(Pitch));
				front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
				direction = glm::normalize(front);
				// also re-calculate the Right and Up vector
				glm::vec3 WorldUp = glm::vec3(0, 1, 0);
				glm::vec3 Right = glm::normalize(glm::cross(direction, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
				up = glm::normalize(glm::cross(Right, direction));
				printf("Dir: %f %f %f\n", direction.x, direction.y, direction.z);
				
				window->damage(1);
				return 1;
			}
			else if (mode == Mode::Pan) {
				float currentX, currentY;
				getMouseNDC(currentX, currentY);
				float offsetX = -(currentX - lastX) * MouseSensitivity * 3;
				float offsetY = -(currentY - lastY) * MouseSensitivity * 3;
				lastX = currentX;
				lastY = currentY;
				glm::vec3 right = glm::normalize(glm::cross(direction, up));
				position += right * offsetX + up * offsetY;

				window->damage(1);
				return 1;
			}
			break;

		case FL_MOUSEWHEEL:
			float zamt = (Fl::event_dy() < 0) ? -1 : 1;
			position += direction * zamt * MouseSensitivity * 0.5f;
			window->damage(1);
			return 1;
			break;

	}
	return 0;
}

void FreeCamera::
getMouseNDC(float& x, float& y)
//==========================================================================
{
	// notice, we put everything into doubles so we can do the math
	float mx = (float)Fl::event_x();	// remeber where the mouse went down
	float my = (float)Fl::event_y();

	// we will assume that the viewport is the same as the window size
	float wd = (float)window->w();
	float hd = (float)window->h();

	// remember that FlTk has Y going the wrong way!
	my = hd - my;

	x = (mx / wd) * 2.0f - 1.f;
	y = (my / hd) * 2.0f - 1.f;
}

void FreeCamera::setWindow(Fl_Gl_Window* w) {
	window = w;
}

glm::vec3 FreeCamera::getPosition() {
	return position;
}
glm::vec3 FreeCamera::getDirection() {
	return direction;
}
glm::vec3 FreeCamera::getUp() {
	return up;
}