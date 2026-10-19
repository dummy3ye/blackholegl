#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <glad/gl.h>

class Shader {
public:
    GLuint id;
    Shader(const std::string& computePath);
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setMat4(const std::string& name, const float* mat) const;
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;
};

#endif
