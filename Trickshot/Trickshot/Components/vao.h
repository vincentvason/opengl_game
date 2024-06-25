#pragma once
#include <glad/glad.h>
#include "VBO.h"

/// <summary>
/// Vertex Array Object's Class
/// </summary>
class VAO
{
public:
	GLuint m_id;
	VAO();

	void enableVertexAttribArray(VBO& vbo, GLuint layout = 0, GLuint num_components = 3, GLenum type = GL_FLOAT, GLsizeiptr stride = 0, void* offset = (void*)0);
	void bindVertexArray();
	void unbindVertexArray();
	void deleteVertexArray();
};