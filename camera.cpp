#include "camera.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, float pitch, float yaw, float roll) {
	this->position = position;
	this->pitch = pitch;
	this->yaw = yaw;
	this->roll = roll;
}

// Get the camera's view matrix, which will be applied to the object
glm::mat4 Camera::GetViewMatrix() {
	glm::mat4 viewMatrix = glm::mat4(1.f);
	//apply series of rotations to the empty matrix to get camera orientation
	viewMatrix = glm::rotate(viewMatrix, pitch, glm::vec3(1.f, 0.f, 0.f));
	viewMatrix = glm::rotate(viewMatrix, yaw, glm::vec3(0.f, 1.f, 0.f));
	viewMatrix = glm::rotate(viewMatrix, roll, glm::vec3(0.f, 0.f, 1.f));

	//translate view matrix by inverse of camera position to ensure it is always at the origin in view space
	//ex. if camera is at (10,0,0) and object is at (20,0,0), we translate by (-10,0,0) so that camera is (0,0,0) in viewspace.
	viewMatrix = glm::translate(viewMatrix, -position);
	return viewMatrix;
}

void Camera::MoveLongitude(float deltaTime, float speed){
	position.x += glm::sin(-yaw) * speed * deltaTime;
	position.z += glm::cos(-yaw) * speed * deltaTime;
}

void Camera::MoveLatitude(float deltaTime, float speed){
	position.x += glm::sin(-yaw + glm::radians(90.f)) * speed * deltaTime;
	position.z += glm::cos(-yaw + glm::radians(90.f)) * speed * deltaTime;
}

void Camera::MoveVertical(float deltaTime, float speed){
	position.y += speed * deltaTime;
}

void Camera::Rotate(float mouseDeltaX, float mouseDeltaY, int displayWidth, int displayHeight){
	float h = float(mouseDeltaX) / float(displayWidth); //normalize horizontal movement
	float v = float(mouseDeltaY) / float(displayHeight); //normalize vertical movement

	yaw = glm::radians((h * 2.f - 1.f) * 75.f); //apply sensitivity
	pitch = glm::radians((v * 2.f - 1.f) * 75.f);

	//clamp camera pitch to prevent 360 veritcal movement
	if (pitch > glm::radians(89.f)) {
		pitch = glm::radians(89.f);
	}
	if (pitch < glm::radians(-89.f)) {
		pitch = glm::radians(-89.f);
	}
}

void Camera::Update(float deltaTime, bool keyW, bool keyA, bool keyS, bool keyD, bool keyZ, bool keyX, bool keyLeftShift, float mouseDeltaX, float mouseDeltaY, int displayWidth, int displayHeight){
	//adjust movement speed if holding down shift
	float cameraSpeed = keyLeftShift ? 5.f : 10.f;

	//update camera based on inputs
	if (keyW) MoveLongitude(deltaTime, -cameraSpeed);
	if (keyS) MoveLongitude(deltaTime, cameraSpeed);
	if (keyA) MoveLatitude(deltaTime, -cameraSpeed);
	if (keyD) MoveLatitude(deltaTime, cameraSpeed);
	if (keyZ) MoveVertical(deltaTime, -cameraSpeed);
	if (keyX) MoveVertical(deltaTime, cameraSpeed);

	//rotate camera
	Rotate(mouseDeltaX, mouseDeltaY, displayWidth, displayHeight);
}
