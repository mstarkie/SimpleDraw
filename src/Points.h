#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Points
{
private:
	const VertexBuffer& m_Vbuffer;
	const IndexBuffer& m_Ibuffer;
public:
	Points(VertexBuffer& vBuffer, IndexBuffer& iBuffer);
	~Points();
	void Draw();
};
