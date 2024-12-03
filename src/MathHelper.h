#pragma once
#include "Utilities/Pnt3f.H"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define PI 3.14159265

namespace MathHelper {
	float lerp(float a, float b, float t) {
		return a + (b - a) * t;
	}

	float degreeToRadians(float degree) {
		return degree * PI / 180.0;
	}

	//we assume front is -z in model space
	glm::mat4 getTransformMatrix(glm::vec3 position, glm::vec3 front, glm::vec3 up, glm::vec3 scale) {
		glm::mat4 transform(1.0f);
		glm::vec3 right = glm::cross(front, up);
		front = glm::normalize(front);
		up = glm::normalize(up);
		right = glm::normalize(right);
		glm::mat4 rotate = glm::mat4(
			right.x,	right.y,	right.z,	0.0f,
			up.x,		up.y,		up.z,		0.0f,
			-front.x,	-front.y,	-front.z,	0.0f,
			0.0f,		0.0f,		0.0f,		1.0f
		);
		transform = glm::translate(transform, position);
		transform = transform * rotate;
		transform = glm::scale(transform, scale);
		return transform;
	}

	void GxM(float* points, float* matrix) {
		float tempPoints[4] = { points[0],points[1],points[2],points[3] };
		for (int i = 0; i < 4; i++) {
			points[i] = 0;
		}

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				points[i] += matrix[j * 4 + i] * tempPoints[j];
			}
		}
	}

	float MxT(float* matrix, float t) {
		float result = 0;
		float points[4] = { t * t * t,t * t,t,1 };
		for (int i = 0; i < 4; i++) {
			result += points[i] * matrix[i];
		}
		return result;
	}

	void mulRotateMatrix(float* matrix, Pnt3f& points) {
		float tempPoints[3] = { points.x,points.y,points.z };
		float* pointsP[3] = { &points.x,&points.y,&points.z };
		for (int i = 0; i < 3; i++) {
			*pointsP[i] = 0;
		}

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				*pointsP[i] += matrix[j * 3 + i] * tempPoints[j];
			}
		}
	}

	float distanceToPlane(Pnt3f pointOnPlane, Pnt3f normalVector, const Pnt3f targetPoint) {
		double a = normalVector.x;
		double b = normalVector.y;
		double c = normalVector.z;
		double d = -(a * pointOnPlane.x + b * pointOnPlane.y + c * pointOnPlane.z);

		double numerator = std::abs(a * targetPoint.x + b * targetPoint.y + c * targetPoint.z + d);
		double denominator = std::sqrt(a * a + b * b + c * c);

		return numerator / denominator;
	}

	float distance(Pnt3f point1, Pnt3f point2) {
		return (point1 + (-1) * point2).len();
	}
}