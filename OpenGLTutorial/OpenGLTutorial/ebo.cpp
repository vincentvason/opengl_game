#include"EBO.h"

/// <summary>
/// Element Buffer Object's Construction
/// </summary>
/// <param name="indices">Vector of indices</param>
EBO::EBO(std::vector<GLuint>& indices)
{
	glGenBuffers(1, &m_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

/// <summary>
/// Bind Element Buffer Object
/// </summary>
void EBO::bindBuffer()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

/// <summary>
/// Unbind Element Buffer Object
/// </summary>
void EBO::unbindBuffer()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/// <summary>
/// Delete Element Buffer Object
/// </summary>
void EBO::deleteBuffer()
{
	glDeleteBuffers(1, &m_id);
}