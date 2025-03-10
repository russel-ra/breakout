#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Shader.h"

Shader::Shader(const std::string& vertexFilepath, const std::string& fragmentFilepath)
    : m_VertexFilepath(vertexFilepath), m_FragmentFilepath(fragmentFilepath), m_ID(0)
{
    std::string vertexSource = ParseShader(vertexFilepath);
    std::string fragmentSource = ParseShader(fragmentFilepath);
    m_ID = CreateShader(vertexSource, fragmentSource);
    glUseProgram(m_ID);
}

Shader::~Shader()
{
    glDeleteProgram(m_ID);
}

void Shader::Bind() const
{
    glUseProgram(m_ID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int v0)
{
    int location = GetUniformLocation(name);
    glUniform1i(location, v0);
}

void Shader::SetUniform3f(const std::string& name, const glm::vec3& vec3)
{
    int location = GetUniformLocation(name);
    glUniform3f(location, vec3.x, vec3.y, vec3.z);
}

void Shader::SetUniform4f(const std::string& name, const glm::vec4& vec4)
{
    int location = GetUniformLocation(name);
    glUniform4f(location, vec4.x, vec4.y, vec4.z, vec4.w);
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
    int location = GetUniformLocation(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.count(name))
    {
        return m_UniformLocationCache[name];
    }

    int location = glGetUniformLocation(m_ID, name.c_str());

    if (location == -1)
        std::cout << "Warning: uniform " << name << " doesn't exist!";
    else
        m_UniformLocationCache[name] = location;

    return location;
}

std::string Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    std::string line;
    std::stringstream ss;
    while (std::getline(stream, line))
    {
        if (line.find("//") != std::string::npos)
        {
            continue;
        }
        else
        {
            ss << line << '\n';
        }
    }

    return ss.str();
}

unsigned int Shader::CompileShader(const std::string& source, unsigned int type)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        // we can't do this cuz this is a stack allocation but length is variable
        //char message[length];

        // alloca allows us to dynamically allocate onto the stack
        char* message = (char*)alloca(length * sizeof(char));

        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "FAILED TO COMPILE SHADER!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0; // we're returning an unsigned int so can't return -1
    }

    return id;
}

// the two string arguments here are the actual shaders
// so openGL will be compiling these two shaders, linking them into a single program and
// then returning an id which is why this function returns an int
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
    unsigned int fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    // no longer need the shaders
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

