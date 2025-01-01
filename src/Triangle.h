#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Triangle
{
private:
	const VertexBuffer& m_Vbuffer;
	const IndexBuffer& m_Ibuffer;
public:
	Triangle(VertexBuffer& vBuffer, IndexBuffer& iBuffer);
	~Triangle();
	void Draw();
};
