#include "Triangle.h"
#include "Renderer.h"

Triangle::Triangle(VertexBuffer& vBuffer, IndexBuffer& iBuffer) :
	m_Vbuffer(vBuffer), m_Ibuffer(iBuffer)
{
}

Triangle::~Triangle()
{
}

void Triangle::Draw()
{
	GlCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0));
	GlCall(glDrawElements(GL_TRIANGLES, m_Vbuffer.Count() / 3, GL_UNSIGNED_INT, nullptr));
}