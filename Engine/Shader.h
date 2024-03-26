#pragma once

#include <glad/glad.h>
#include <glm/ext.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
private:
	uint32_t m_programID;

public:
	Shader();

	void load(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	void activate();

	void setUniform(const std::string& uniformName, bool value);
	void setUniform(const std::string& uniformName, int32_t value);
	void setUniform(const std::string& uniformName, float_t value);
	void setUniform(const std::string& uniformName, const glm::vec2& value);
	void setUniform(const std::string& uniformName, const glm::vec3& value);
	void setUniform(const std::string& uniformName, const glm::vec4& value);
	void setUniform(const std::string& uniformName, const glm::mat2& value);
	void setUniform(const std::string& uniformName, const glm::mat3& value);
	void setUniform(const std::string& uniformName, const glm::mat4& value);
};