#pragma once
#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>


class ShaderHandler {
public:
	ShaderHandler();

	GLuint uProjection;
	GLuint uView;
	GLuint uClip;
	GLuint uHeightmap;

	void LoadShaders(std::string vertexShaderFile, std::string fragmentShaderFile);
	void Enable();
	void Disable();
	void Destroy();

	GLuint GetUniformLocation(std::string name);
	void SetUniformInt(GLuint location, int value);
	void SetUniformFloat(GLuint location, float value);
	void SetUniformVec2(GLuint location, glm::vec2& value);
	void SetUniformVec3(GLuint location, glm::vec3& value);
	void SetUniformVec4(GLuint location, glm::vec4& value);
	void SetUniformMatrix4(GLuint location, glm::mat4& value);
	void SetUniformSampler2D(GLuint location, GLenum texture, GLuint textureID);
	void BindAttribute(int attribute, std::string variableName);
	void SetClip(glm::vec4 clip);
	void SetView(glm::mat4 view);
	void SetProjection(glm::mat4 projection);


private:
	GLuint m_shaderProgramID;
	GLuint m_vertexShaderID;
	GLuint m_fragmentShaderID;

	GLuint CompileShader(GLenum shaderType, std::string filePath);
	
};