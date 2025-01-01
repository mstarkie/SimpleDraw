#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Lines
{
private:
	const VertexBuffer& m_Vbuffer;
	const IndexBuffer& m_Ibuffer;
	const int mode;
public:
	Lines(VertexBuffer& vBuffer, IndexBuffer& iBuffer, int mode);
	~Lines();
	void Draw();
};
