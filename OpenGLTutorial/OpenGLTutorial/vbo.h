#pragma once
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texture_uv;
};

/// <summary>
/// Vertex Buffer Object
/// </summary>
class VBO
{
public:
	GLuint m_id;
	VBO(std::vector<Vertex>& vertices);
	VBO(std::vector<glm::mat4>& matrices);

	void bindBuffer();
	void unbindBuffer();
	void deleteBuffer();
};