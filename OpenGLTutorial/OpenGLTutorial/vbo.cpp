#include"VBO.h"
#include<iostream>

/// <summary>
/// Vertex Buffer Object's Constructor
/// </summary>
/// <param name="vertices">Vector of Vertices</param>
VBO::VBO(std::vector<Vertex>& vertices)
{
	glGenBuffers(1, &m_id);
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

/// <summary>
/// Vertex Buffer Object's Constructor
/// </summary>
/// <param name="mat4s">Instances Matrices</param>
VBO::VBO(std::vector<glm::mat4>& mat4s)
{
	glGenBuffers(1, &m_id);
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	glBufferData(GL_ARRAY_BUFFER, mat4s.size() * sizeof(Vertex), mat4s.data(), GL_STATIC_DRAW);
}

/// <summary>
/// Bind Vertex Buffer Object
/// </summary>
void VBO::bindBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

/// <summary>
/// Unbind Vertex Buffer Object
/// </summary>
void VBO::unbindBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/// <summary>
/// Delete Vertex Buffer Object
/// </summary>
void VBO::deleteBuffer()
{
	glDeleteBuffers(1, &m_id);
}