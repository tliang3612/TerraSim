#include <glm/glm.hpp>
#include "../camera/camera.h"
#include "../terrain/terrain.h"

struct Raycaster {

	glm::vec3 static GetRaycastFromCameraToTerrain(glm::vec3 cameraPos, Terrain& terrain, float ndcX, float ndcY, glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {
		glm::vec4 clipSpaceCoords(ndcX, ndcY, -1.f, 1.f);
		glm::mat4 inverseProjection = glm::inverse(projectionMatrix);
		glm::vec4 viewSpaceCoords = inverseProjection * clipSpaceCoords; //unproject the screen space coordinates to get view space
		viewSpaceCoords.z = -1.f;
		viewSpaceCoords.w = 0.f;

		glm::mat4 inverseView = glm::inverse(viewMatrix);
		glm::vec4 worldCoords = inverseView * viewSpaceCoords;
		glm::vec3 rayDirection = glm::normalize(glm::vec3(worldCoords)); //go from view space to world space to get mouse ray direction'

		int maxSteps = 2000;
		float epsilon = 0.001f;

		// cast a ray from camera to first object. using ray marching algorithm
		glm::vec3 currentPosition = cameraPos;
		glm::vec3 intersectionPoint = glm::vec3(std::numeric_limits<float>::max());
		for (int i = 0; i < maxSteps; i++) {

			float distanceToTerrain = currentPosition.y - terrain.GetHeightFromWorld(currentPosition.x, currentPosition.z);

			// march ray
			currentPosition += rayDirection * distanceToTerrain;

			// if the ray is close enough to the terrain, intersection is found, exit the loop
			if (distanceToTerrain < epsilon) {
				intersectionPoint = currentPosition;
				return intersectionPoint;
			}
		}

		return glm::vec3(std::numeric_limits<float>::max());
	}
};