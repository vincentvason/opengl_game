#include"VAO.h"

/// <summary>
/// Vertex Array Object's Constructor
/// </summary>
VAO::VAO()
{
	glGenVertexArrays(1, &m_id);
}

/// <summary>
/// Enable Vertex Array Object
/// </summary>
/// <seealso href="https://youtu.be/YaiSvKTOeRg?si=pdgbl48V5LKZURLn&amp;t=200">
/// Vertex Attribute Pointer Explanation
/// </seealso>
/// <param name="vbo">VBO that binding to array</param> 
/// <param name="layout">layout of attribute in Vertex's shader file</param>
/// <param name="num_components">number of component in each vertex</param>
/// <param name="type">data type of component</param>
/// <param name="stride">byte distance between each vertex</param>
/// <param name="offset">byte distance between each layout</param>
void VAO::enableVertexAttribArray(VBO& vbo, GLuint layout, GLuint num_components, GLenum type, GLsizeiptr stride, void* offset)
{
	vbo.bindBuffer();
	// Configure and enable vertex attributes to know how to read and use VBO
	glVertexAttribPointer(layout, num_components, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	vbo.unbindBuffer();
}

/// <summary>
/// Bind Vertex Array Object
/// </summary>
void VAO::bindVertexArray()
{
	glBindVertexArray(m_id);
}

/// <summary>
/// Unbind Vertex Array Object
/// </summary>
void VAO::unbindVertexArray()
{
	glBindVertexArray(0);
}

/// <summary>
/// Delete Vertex Array Object
/// </summary>
void VAO::deleteVertexArray()
{
	glDeleteVertexArrays(1, &m_id);
}