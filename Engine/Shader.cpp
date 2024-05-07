#include "Shader.h"

Shader::Shader()
{
	m_programID = -1;
}

void Shader::load(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexShaderPath);
        fShaderFile.open(fragmentShaderPath);
        std::stringstream vShaderStream, fShaderStream;

        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    unsigned int vertex, fragment;

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    // shader Program
    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertex);
    glAttachShader(m_programID, fragment);
    glLinkProgram(m_programID);

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::activate()
{
	glUseProgram(m_programID);
}

void Shader::disable()
{
    glUseProgram(0);
}

void Shader::setUniform(const std::string& uniformName, bool value)
{
	glUniform1i(glGetUniformLocation(m_programID, uniformName.c_str()), (int32_t)value);
}

void Shader::setUniform(const std::string& uniformName, int32_t value)
{
    glUniform1i(glGetUniformLocation(m_programID, uniformName.c_str()), value);
}

void Shader::setUniform(const std::string& uniformName, float_t value)
{
    glUniform1f(glGetUniformLocation(m_programID, uniformName.c_str()), (int)value);
}

void Shader::setUniform(const std::string& uniformName, const glm::vec2& value)
{
    glUniform2fv(glGetUniformLocation(m_programID, uniformName.c_str()), 1, &value[0]);
}

void Shader::setUniform(const std::string& uniformName, const glm::vec3& value)
{
    glUniform3fv(glGetUniformLocation(m_programID, uniformName.c_str()), 1, &value[0]);
}

void Shader::setUniform(const std::string& uniformName, const glm::vec4& value)
{
    glUniform4fv(glGetUniformLocation(m_programID, uniformName.c_str()), 1, &value[0]);
}

void Shader::setUniform(const std::string& uniformName, const glm::mat2& value)
{
    glUniformMatrix2fv(glGetUniformLocation(m_programID, uniformName.c_str()), 1, false, &value[0][0]);
}

void Shader::setUniform(const std::string& uniformName, const glm::mat3& value)
{
    glUniformMatrix3fv(glGetUniformLocation(m_programID, uniformName.c_str()), 1, false, &value[0][0]);
}

void Shader::setUniform(const std::string& uniformName, const glm::mat4& value)
{
    glUniformMatrix4fv(glGetUniformLocation(m_programID, uniformName.c_str()), 1, false, &value[0][0]);
}
