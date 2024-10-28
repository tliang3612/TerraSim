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
	glm::vec3 position;
	float pitch;
	float yaw;
	float roll;
	void Update(float deltaTime, bool keyW, bool keyA, bool keyS, bool keyD, bool keyZ, bool keyX, bool keyLeftShift, float mouseDeltaX, float mouseDeltaY, int displayWidth, int displayHeight);

private:
	void MoveLongitude(float deltaTime, float speed);
    void MoveLatitude(float deltaTime, float speed);
    void MoveVertical(float deltaTime, float speed);
    void Rotate(float mouseDeltaX, float mouseDeltaY, int displayWidth, int displayHeight);
}; 
