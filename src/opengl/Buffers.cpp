#include "Buffers.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

VertexBuffer::VertexBuffer(const void* data, GLsizei size)
{
    glCreateBuffers(1, &m_BufferId);
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}


VertexBuffer :: ~VertexBuffer()
{
    glDeleteBuffers(1, &m_BufferId);
}


void VertexBuffer::AddData(const void* data, GLsizei size)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}


void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_BufferId);
}


void VertexBuffer::UnBind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}



//index buffer
IndexBuffer::IndexBuffer(const GLuint* data, GLsizei count)
{
    glCreateBuffers(1, &m_BufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}


IndexBuffer :: ~IndexBuffer()
{
    glDeleteBuffers(1, &m_BufferId);
}


void IndexBuffer::AddData(const GLuint* data, GLsizei count) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}

void IndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferId);
}

void IndexBuffer::UnBind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}



void VertexArray::BindAttribute(GLuint program, GLuint location, const std::string& name) {
    glBindAttribLocation(program, location, name.c_str());
}

Texture::Texture(int _height, int _width) : height(_height), width(_width) {
    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_FLOAT, 0);
}

Texture::Texture(const std::string& filename) : width(0), height(0) {
    stbi_set_flip_vertically_on_load(true);
    
    int numofcompenets;
    unsigned char* imagedata = stbi_load(filename.c_str(), &width, &height, &numofcompenets, 4);

    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imagedata);
}

inline GLuint Texture::GetId() const {
    return m_TextureID;
}

void Texture::bind(GLuint slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

Texture::~Texture() {
    glDeleteTextures(1, &m_TextureID);
}

FrameBuffer::FrameBuffer(int width, int height) : m_FrameTexture(height,width){
    glGenFramebuffers(1, &m_FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FrameTexture.GetId(), 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Bind() const{
    glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
}

void FrameBuffer::UnBInd() const{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::BindTexture(int slot) const{
    m_FrameTexture.bind(slot);
}