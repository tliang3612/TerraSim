#include "../util/util.h"
#include "shader_handler.h"

//load and compile the vertex and fragment shader
void ShaderHandler::LoadShaders(std::string vertexShaderFile, std::string fragmentShaderFile) {
    m_vertexShaderID = CompileShader(GL_VERTEX_SHADER, vertexShaderFile);
    m_fragmentShaderID = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderFile);
    m_shaderProgramID = glCreateProgram();
    if (m_shaderProgramID == 0) {
        util::fatal_error("Unable to create shader program\n", nullptr);
    }

    glAttachShader(m_shaderProgramID, m_vertexShaderID);
    glAttachShader(m_shaderProgramID, m_fragmentShaderID);

    GLint success = 0;
    GLchar errorLog[1024] = { 0 };

    glLinkProgram(m_shaderProgramID);
    glGetProgramiv(m_shaderProgramID, GL_LINK_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(m_shaderProgramID, sizeof(errorLog), nullptr, errorLog);
        util::fatal_error("Error linking shader program: '%s'\n", errorLog);
    }

    glValidateProgram(m_shaderProgramID);
    glGetProgramiv(m_shaderProgramID, GL_VALIDATE_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(m_shaderProgramID, sizeof(errorLog), nullptr, errorLog);
        util::fatal_error("Invalid shader program: '%s'\n", errorLog);
    }
}

GLuint ShaderHandler::CompileShader(GLenum shaderType, std::string filePath)
{
    //read shader file given filePath and compile shader code
	std::string contents = util::read_file(filePath);
	const char* source = contents.c_str();
	int length = strlen(source);
	GLuint shaderObject = glCreateShader(shaderType);
	glShaderSource(shaderObject, 1, &source, &length);
	glCompileShader(shaderObject);

    //check for compilation errors
    GLint success;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderObject, sizeof(infoLog), nullptr, infoLog);
        glDeleteShader(shaderObject);
        util::fatal_error("Error Compiling Shader at %s: %s", filePath.c_str(), infoLog);
    }
    return shaderObject;
}

//get the location of a uniform variable in this shader.
GLuint ShaderHandler::GetUniformLocation(std::string name) {
	return glGetUniformLocation(m_shaderProgramID, name.c_str());
}

void ShaderHandler::SetUniformFloat(GLuint location, float value) {
	glUniform1f(location, value);
}

void ShaderHandler::SetUniformInt(GLuint location, int value) {
	glUniform1i(location, value);
}

void ShaderHandler::SetUniformVec2(GLuint location, glm::vec2& value) {
	glUniform2f(location, value.x, value.y);
}

void ShaderHandler::SetUniformVec3(GLuint location, glm::vec3& value) {
	glUniform3f(location, value.x, value.y, value.z);
}

void ShaderHandler::SetUniformVec4(GLuint location, glm::vec4& value) {
	glUniform4f(location, value.x, value.y, value.z, value.w);
}

void ShaderHandler::SetUniformMatrix4(GLuint location, glm::mat4& value) {
	glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

void ShaderHandler::SetUniformSampler2D(GLuint location, GLenum texture, GLuint textureID) {
    glActiveTexture(texture);
    glEnable(GL_TEXTURE_2D);
    glUniform1i(location, texture - GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

//bind an attribute to a shader variable.
void ShaderHandler::BindAttribute(int attribute, std::string variableName) {
	glBindAttribLocation(m_shaderProgramID, attribute, variableName.c_str());
}

void ShaderHandler::Enable() {
	glUseProgram(m_shaderProgramID);
}

void ShaderHandler::Disable() {
	glUseProgram(0);
}

//delete all shaders
void ShaderHandler::Destroy() {
	Disable();
	glDetachShader(m_shaderProgramID, m_vertexShaderID);
	glDetachShader(m_shaderProgramID, m_fragmentShaderID);
	glDeleteShader(m_vertexShaderID);
	glDeleteShader(m_fragmentShaderID);
	glDeleteProgram(m_shaderProgramID);
}