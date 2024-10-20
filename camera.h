#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// A camera.
class Camera {
public:
	Camera() = default;

	Camera(glm::vec3 position, float pitch, float yaw, float roll);

	// Get the camera's view matrix.
	glm::mat4 GetViewMatrix();
	void SetPitch(float pitch) { m_pitch = pitch; }
	void SetYaw(float yaw) { m_yaw = yaw; }
	const glm::vec3 GetPosition() { return position; }
	const glm::vec3 GetRotation() { return glm::vec3(m_pitch, m_yaw, m_roll); }
	glm::vec3 position;

private:
	float m_pitch;
	float m_yaw;
	float m_roll;

}; 
