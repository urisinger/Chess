#pragma once
#include <string>
#include <GLAD/glad.h>
#include <unordered_map>


class Shader {
public:
	Shader(const std::string& vertexFilePath, const std::string& fragmentFilePath);
	~Shader();

	GLuint GetID();
	void Bind();
	void UnBind();

    GLuint Location(const std::string& Name);

private:
	GLuint m_Renderer_ID;

    std::unordered_map<std::string, int> m_Locations;
	GLuint CompileShader(GLuint type, const std::string& source);
	std::string OpenShader(std::string filepath);
};