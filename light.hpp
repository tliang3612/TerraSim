struct Light {

	glm::vec3 lightPosition;
	glm::vec3 lightDirection;
	float brightness;
	float sunFalloff;
	float sunIntensity;

	Light() = default;

	Light(glm::vec3 lightPosition, glm::vec3 lightDirection, float lightIntensity, float sunFalloff, float sunIntensity) {
		this->lightPosition = lightPosition;
		this->lightDirection = lightDirection;
		this->brightness = lightIntensity;
		this->sunFalloff = sunFalloff;
		this->sunIntensity = sunIntensity;
	}

	glm::mat4 GetViewMatrix() {
		return glm::lookAt(lightPosition, lightDirection, glm::vec3(0.f, 1.f, 0.f));
	}

	glm::mat4 GetProjectionMatrix() {
		//orthographic projection matrix
		float orthoSize = 100.0f;

		return glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, 0.1f, 500.f);
	}
};