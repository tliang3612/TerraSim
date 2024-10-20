#include "camera.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, float pitch, float yaw, float roll) {
	this->position = position;
	m_pitch = pitch;
	m_yaw = yaw;
	m_roll = roll;
}

// Get the camera's view matrix, which will be applied to the object
glm::mat4 Camera::GetViewMatrix() {
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	//apply series of rotations to the empty matrix to get camera orientation
	viewMatrix = glm::rotate(viewMatrix, m_pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, m_yaw, glm::vec3(0.0f, 1.0f, 0.0f));
	viewMatrix = glm::rotate(viewMatrix, m_roll, glm::vec3(0.0f, 0.0f, 1.0f));

	//translate view matrix by inverse of camera position to ensure it is always at the origin in view space
	//ex. if camera is at (10,0,0) and object is at (20,0,0), we translate by (-10,0,0) so that camera is (0,0,0) in viewspace.
	//object becomes (10,0,0)
	viewMatrix = glm::translate(viewMatrix, -position);
	return viewMatrix;
}