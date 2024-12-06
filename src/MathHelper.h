#pragma once
#include "Utilities/Pnt3f.H"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <cmath>

namespace MathHelper {
	float lerp(float a, float b, float t);

	glm::vec3 lerpVec3(glm::vec3 a, glm::vec3 b, float t);

	float clamp(float value, float min, float max);

	//get the gradient color in [colorA, colorB, color C], g is colorB transition point
	//t = 0 -> colorA
	//t = g -> colorB
	//t = 1 -> colorC
	glm::vec3 gradientColor(glm::vec3 colorA, glm::vec3 colorB, glm::vec3 colorC, float g, float t);

	float degreeToRadians(float degree);

	float dot(Pnt3f P1, Pnt3f P2);

	//we assume front is -z in model space
	glm::mat4 getTransformMatrix(glm::vec3 position, glm::vec3 front, glm::vec3 up, glm::vec3 scale);

	void GxM(float* points, float* matrix);

	float MxT(float* matrix, float t);

	void mulRotateMatrix(float* matrix, Pnt3f& points);

	float distanceToPlane(Pnt3f pointOnPlane, Pnt3f normalVector, const Pnt3f targetPoint);

	float distance(Pnt3f point1, Pnt3f point2);

	bool segmentIntersectCircle(
		const Pnt3f P1, const Pnt3f& P2,
		const Pnt3f& center, const Pnt3f& normal, float radius);

	//return random float, range [0, 1)
	float randomFloat();

	// 隨機生成球面錐形上的方向向量
	glm::vec3 randomDirectionInCone(const glm::vec3& direction, float angleDegrees);
}