#include "Points.h"
#include "Renderer.h"

Points::Points(VertexBuffer& vBuffer, IndexBuffer& iBuffer) : m_Vbuffer(vBuffer), m_Ibuffer(iBuffer)
{
}

Points::~Points()
{
}

void Points::Draw()
{
	GlCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0)); // tell GL the vertices start at idx 0 and are 2 floats long.
	GlCall(glDrawElements(GL_POINTS, m_Vbuffer.Count(), GL_UNSIGNED_INT, nullptr)); // GL state machine knows the data to be drawn is in buffer.
}
 