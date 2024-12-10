#include "MathHelper.h"

#define PI 3.14159265

namespace MathHelper {
	float lerp(float a, float b, float t) {
		return a + (b - a) * t;
	}

	glm::vec3 lerpVec3(glm::vec3 a, glm::vec3 b, float t) {
		return a + (b - a) * t;
	}
	Pnt3f lerpVec3(Pnt3f a, Pnt3f b, float t) {
		return a + (b - a) * t;
	}

	float clamp(float value, float min, float max) {
		if (value < min) return min;
		if (value > max) return max;
		return value;
	}

	float sigmoid(float x, float k = 10) {
		return 1 / (1 + pow(2.71828182846, -k * (x - 0.5)));
	}

	int quadrant(float x, float y) {
		if (y >= 0)
			if (x >= 0)
				return 1;
			else
				return 2;
		else
			if (x >= 0)
				return 4;
			else
				return 3;
	}

	//get the gradient color in [colorA, colorB, color C], g is colorB position in gradient line
	//t = 0 -> colorA
	//t = g -> colorB
	//t = 1 -> colorC
	glm::vec3 gradientColor(glm::vec3 colorA, glm::vec3 colorB, glm::vec3 colorC, float g, float t) {
		t = clamp(t, 0.0f, 1.0f); // 確保 t 在 [0, 1] 範圍內

		if (g <= 0.0f) {
			// 只在 B 和 C 之間插值
			return lerpVec3(colorB, colorC, t);
		}
		else if (g >= 1.0f) {
			// 只在 A 和 B 之間插值
			return lerpVec3(colorA, colorB, t);
		}
		else if (t <= g) {
			// 在 A 和 B 之間插值
			float normalizedT = t / g;
			return lerpVec3(colorA, colorB, normalizedT);
		}
		else {
			// 在 B 和 C 之間插值
			float normalizedT = (t - g) / (1.0f - g);
			return lerpVec3(colorB, colorC, normalizedT);
		}
	}

	float degreeToRadians(float degree) {
		return degree * PI / 180.0;
	}

	float dot(Pnt3f P1, Pnt3f P2) {
		return P1.x * P2.x + P1.y * P2.y + P1.z * P2.z;
	}

	//we assume front is -z in model space
	glm::mat4 getTransformMatrix(glm::vec3 position, glm::vec3 front, glm::vec3 up, glm::vec3 scale) {
		glm::mat4 transform(1.0f);
		glm::vec3 right = glm::cross(front, up);
		front = glm::normalize(front);
		up = glm::normalize(up);
		right = glm::normalize(right);
		glm::mat4 rotate = glm::mat4(
			right.x, right.y, right.z, 0.0f,
			up.x, up.y, up.z, 0.0f,
			-front.x, -front.y, -front.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
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

	bool segmentIntersectCircle(
		const Pnt3f P1, const Pnt3f& P2,
		const Pnt3f& center, const Pnt3f& normal, float radius)
	{
		Pnt3f lineDir = P2 - P1;
		float denom = dot(lineDir, normal);

		// Step 1: Check if line is parallel to the plane
		if (abs(denom) < 1e-6) {
			return false; // Line is parallel to the plane, no intersection
		}

		// Step 2: Find intersection point
		float t = dot(center - P1, normal) * (1 / denom);

		// Check if the intersection is within the segment
		if (t < 0.0f || t > 1.0f) {
			return false; // Intersection is outside the segment
		}

		Pnt3f intersectPoint = P1 + t * lineDir;

		// Step 3: Check if intersection is inside the circle
		return (intersectPoint - center).len2() <= radius * radius;
	}

	//return random float, range [0, 1)
	float randomFloat() {
		return (float)rand() / (RAND_MAX + 1.0);
	}

	// 隨機生成球面錐形上的方向向量
	glm::vec3 randomDirectionInCone(const glm::vec3& direction, float angleDegrees) {
		// 確保方向向量為單位向量
		glm::vec3 normalizedDirection = glm::normalize(direction);

		// 將角度轉換為弧度
		float angleRadians = glm::radians(angleDegrees);

		// 隨機生成球坐標參數
		float phi = lerp(0, 2 * PI, randomFloat()); //[0, 2*pi]
		float cosTheta = lerp(cos(angleRadians), 1, randomFloat()); //[cos(andgleDegress), 1]
		float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);

		// 初始生成的向量（以z軸為中心）
		glm::vec3 randomVector(
			sinTheta * std::cos(phi),
			sinTheta * std::sin(phi),
			cosTheta
		);

		// 計算旋轉矩陣將z軸對齊到direction
		glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
		if (glm::length(glm::cross(up, normalizedDirection)) < 1e-6f) {
			up = glm::vec3(1.0f, 0.0f, 0.0f); // 特殊情況處理
		}
		glm::vec3 axis = glm::normalize(glm::cross(up, normalizedDirection));
		float angleToRotate = std::acos(glm::dot(up, normalizedDirection));
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), angleToRotate, axis);

		// 轉換初始向量到錐形方向
		glm::vec4 rotatedVector = rotationMatrix * glm::vec4(randomVector, 0.0f);

		return glm::vec3(rotatedVector);
	}
}