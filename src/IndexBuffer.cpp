#include "IndexBuffer.h"
#include "Renderer.h"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
	: m_Count(count)
{
	ASSERT(sizeof(unsigned int) == sizeof(GLuint));

	GlCall(glGenBuffers(1, &m_RendererID));
	GlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
	GlCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
	GlCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	GlCall(glDeleteBuffers(1, &m_RendererID));
}
