#pragma once
#include <GL/glew.h>

class VertexBuffer
{
private:
	/* every object you create in OpenGL requires an ID */
	unsigned int v_RendererID;
	const int v_Count;
public:
	VertexBuffer(const float* data, int count);
	~VertexBuffer();
	int Count() const { return v_Count; }
};
