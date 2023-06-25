#include "gui/Shaders.h"
#include <iostream>
#include <fstream>


Shader::Shader(const std::string& vertex_file_path, const std::string& fragment_file_path)
{
    // Create the shader program
    m_Renderer_ID = glCreateProgram();

    // Compile the vertex and fragment shaders
    GLuint vertex_shader = CompileShader(GL_VERTEX_SHADER, OpenShader(vertex_file_path));
    GLuint fragment_shader = CompileShader(GL_FRAGMENT_SHADER, OpenShader(fragment_file_path));

    // Attach the shaders to the program and link it
    glAttachShader(m_Renderer_ID, vertex_shader);
    glAttachShader(m_Renderer_ID, fragment_shader);
    glLinkProgram(m_Renderer_ID);
    glValidateProgram(m_Renderer_ID);



    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    glUseProgram(m_Renderer_ID);
}

GLuint Shader::GetID() {
    return m_Renderer_ID;
}
void Shader::Bind() {
    glUseProgram(m_Renderer_ID);
}

void Shader::UnBind() {
    glUseProgram(0);
}


Shader::~Shader() {
    glDeleteProgram(m_Renderer_ID);
}



std::string Shader::OpenShader(std::string filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if (!fileStream.is_open()) {
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return "";
    }

    std::string line = "";
    while (!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}

GLuint Shader::CompileShader(GLuint type, const std::string& source) {
    GLuint id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* messge = (char*)malloc(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, messge);
        std::cout << "failt to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << messge << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

GLuint Shader::Location(const std::string& name){
    if(m_Locations.find(name) != m_Locations.end()){
        return m_Locations[name];
    }

    GLuint location = glGetUniformLocation(m_Renderer_ID,name.c_str());
    m_Locations[name] = location;

    return location;
}

