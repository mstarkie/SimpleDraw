#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer(const float* data, int count) : v_Count(count)
{
	GlCall(glGenBuffers(1, &v_RendererID));
	GlCall(glBindBuffer(GL_ARRAY_BUFFER, v_RendererID));
	GlCall(glBufferData(GL_ARRAY_BUFFER, v_Count * sizeof(float), data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
	GlCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GlCall(glDeleteBuffers(1, &v_RendererID));
}

