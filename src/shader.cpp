#include "shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

static GLuint compileShaderStage(GLenum type, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return 0;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();
    const char* src = code.c_str();

    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> infoLog(logLen);
        glGetShaderInfoLog(shader, logLen, nullptr, infoLog.data());
        std::cerr << "Shader compile error (" << filepath << "):\n" << infoLog.data() << std::endl;
    }
    return shader;
}

Shader::Shader(const std::string& computePath) {
    GLuint comp = compileShaderStage(GL_COMPUTE_SHADER, computePath);
    id = glCreateProgram();
    glAttachShader(id, comp);
    glLinkProgram(id);

    GLint success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLen = 0;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> infoLog(logLen);
        glGetProgramInfoLog(id, logLen, nullptr, infoLog.data());
        std::cerr << "Compute program link error (" << computePath << "):\n" << infoLog.data() << std::endl;
    }
    glDeleteShader(comp);
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    GLuint vert = compileShaderStage(GL_VERTEX_SHADER, vertexPath);
    GLuint frag = compileShaderStage(GL_FRAGMENT_SHADER, fragmentPath);
    id = glCreateProgram();
    glAttachShader(id, vert);
    glAttachShader(id, frag);
    glLinkProgram(id);

    GLint success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLen = 0;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> infoLog(logLen);
        glGetProgramInfoLog(id, logLen, nullptr, infoLog.data());
        std::cerr << "Shader program link error:\n" << infoLog.data() << std::endl;
    }
    glDeleteShader(vert);
    glDeleteShader(frag);
}

void Shader::use() const {
    glUseProgram(id);
}

void Shader::setVec2(const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
}

void Shader::setMat4(const std::string& name, const float* mat) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, mat);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}
