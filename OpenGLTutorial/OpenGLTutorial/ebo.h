#pragma once
#include<vector>
#include<glad/glad.h>


/// <summary>
/// Element Buffer Object
/// </summary>
class EBO
{
public:
	GLuint m_id;
	EBO(std::vector<GLuint>& indices);

	void bindBuffer();
	void unbindBuffer();
	void deleteBuffer();
};
