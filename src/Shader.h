#pragma once

#include <string>
#include <unordered_map>

#include "glm/glm.hpp"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	unsigned int m_ID;
	std::string m_VertexFilepath;
	std::string m_FragmentFilepath;
	std::unordered_map<std::string, int> m_UniformLocationCache;

	int GetUniformLocation(const std::string&);

	std::string ParseShader(const std::string& filepath);
	unsigned int CompileShader(const std::string& source, unsigned int type);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
public:
	Shader(const std::string& vertexFilepath, const std::string& fragmentFilepath);
	~Shader();

	// It's not called "Bind" for shaders in OpenGL
	// This is just to remain consistent with the other classes
	void Bind() const;
	void Unbind() const;

	// Ideally we have a shader system that determines the type of the value we pass it
	// Also ideally we have a maths library that has a vec4 struct
	void SetUniform1i(const std::string& name, int v0);
	void SetUniform3f(const std::string& name, const glm::vec3& vec3);
	void SetUniform4f(const std::string& name, const glm::vec4& vec4);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix);
};

