#include "Lines.h"
#include "Renderer.h"

Lines::Lines(VertexBuffer& vBuffer, IndexBuffer& iBuffer, int mode) :
	m_Vbuffer(vBuffer), m_Ibuffer(iBuffer), mode(mode)
{
}

Lines::~Lines()
{
}

void Lines::Draw()
{
	GlCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0));
	GlCall(glDrawElements(mode, m_Vbuffer.Count() / 2, GL_UNSIGNED_INT, nullptr));
}
