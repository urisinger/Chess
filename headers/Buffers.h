#pragma once
#include <GLAD/glad.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>


template <typename T>
inline GLenum GetGLType() {
    // Default to GL_FLOAT if the type is not specialized
    return GL_FLOAT;
}

template <>
inline GLenum GetGLType<int>() {
    return GL_INT;
}

template <>
inline GLenum GetGLType<unsigned int>() {
    return GL_UNSIGNED_INT;
}

template <>
inline GLenum GetGLType<float>() {
    return GL_FLOAT;
}

template <>
inline GLenum GetGLType<unsigned char>() {
    return GL_UNSIGNED_BYTE;
}


class VertexBuffer
{
private:
	GLuint m_BufferId;
public:
	VertexBuffer(const void* data, GLsizei size);
	~VertexBuffer();

	void AddData(const void* data, GLsizei size);
	void Bind() const;
	void UnBind() const;

};



class IndexBuffer
{
private:
	unsigned int m_BufferId;
public:
	IndexBuffer(const GLuint* data, GLsizei count);
	~IndexBuffer();

	void AddData(const GLuint* data, GLsizei count);
	void Bind() const;
	void UnBind() const;
      
};

struct VertexAttrib {
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    GLsizei offset;

    VertexAttrib(GLsizei _size, GLenum _type, GLuint StrideOffset, GLboolean _normalized, GLsizei _offset) : size(_size), type(_type), normalized(_normalized), offset(_offset) {
        unsigned int typesize;

        switch (_type) {
        case GL_INT:
            typesize = 4;
            break;
        case GL_UNSIGNED_INT:
            typesize = 4;
            break;
        case GL_FLOAT:
            typesize = 4;
            break;
        case GL_UNSIGNED_BYTE:
            typesize = 1;
            break;
        default:
            typesize = 0;
            break;
        }

        stride = (size + StrideOffset) * typesize;
        offset *= typesize;
    }
};


class VertexArray {
public:
    VertexArray() : m_ArrayID(0) { glGenVertexArrays(1, &m_ArrayID); };
    ~VertexArray() { glDeleteVertexArrays(1, &m_ArrayID); };

    template <typename T>
    void AddAttribute(GLint size, GLuint strideOffset, GLboolean normalized, GLsizei offset, std::shared_ptr<VertexBuffer> buf, int location) {
        VertexAttrib attrib(size, GetGLType<T>(), strideOffset, normalized, offset);
        Attributes.emplace(location, std::make_pair(attrib, buf));

        Bind();
        buf->Bind();
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, size, GetGLType<T>(), normalized, attrib.stride, (void*)(intptr_t)attrib.offset);
        buf->UnBind();
        UnBind();
    }

    void BindAttribute(GLuint program ,GLuint location, const std::string& name);

    void Bind() const { glBindVertexArray(m_ArrayID); }

    void UnBind() const { glBindVertexArray(0); }



private:
    std::unordered_map<int,std::pair<VertexAttrib, std::shared_ptr<VertexBuffer>>> Attributes;

    GLuint m_ArrayID;

};

class Texture {
public:
	Texture(const std::string& filename);
	Texture(int height, int width);
	~Texture();

	GLuint GetId() const;
	void bind(unsigned int slot) const;
	void unbind() const;

private:
	int width, height;
	GLuint m_TextureID;
};


class FrameBuffer 
{
public:
	FrameBuffer(int width, int height);


	void Bind() const;

	void UnBInd() const;

	void BindTexture(int slot) const;
private:
	GLuint m_FrameBuffer;
	Texture m_FrameTexture;

	int width, height;
};

