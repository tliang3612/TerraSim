#pragma once
#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>


class ShaderHandler {
public:
	void LoadShaders(std::string vertexShaderFile, std::string fragmentShaderFile);
	void Enable();
	void Disable();
	void Destroy();

	const GLuint GetUniformLocation(std::string name) const;
	void SetUniformInt(GLuint location, int value);
	void LoadUniformFloat(GLuint location, float value);
	void LoadUniformVec2(GLuint location, glm::vec2& value);
	void LoadUniformVec3(GLuint location, glm::vec3& value);
	void LoadUniformVec4(GLuint location, glm::vec4& value);
	void LoadUniformMatrix4(GLuint location, glm::mat4& value);
	void LoadUniformSampler2D(GLuint location, GLenum texture, GLuint textureID);
	void LoadUniformSamplerCube(GLuint location, GLenum texture, GLuint cubemapTextureID);
	void BindAttribute(int attribute, std::string variableName);


private:
	GLuint m_shaderProgramID;
	GLuint m_vertexShaderID;
	GLuint m_fragmentShaderID;

	GLuint CompileShader(GLenum shaderType, std::string filePath);
	
};