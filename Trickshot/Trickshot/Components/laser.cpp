#include "laser.h"

Laser::Laser(float weight)
{
	
	std::vector<Vertex> vertices =
	{
		Vertex{glm::vec3(-0.005f - 0.04f, 0.02f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(-0.005f, 0.0f, 8.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(0.005f, 0.0f, 8.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(0.005f - 0.04f, 0.02f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
	};
	

	/*
	std::vector<Vertex> vertices =
	{
		Vertex{glm::vec3(-0.1f, -0.1f, 0.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(-0.1f, 0.1f, 0.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(0.1f, 0.1f, 0.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(0.1f, -0.1f, 0.5f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
	};
	*/
	std::vector<GLuint> indices = { 0, 2, 1, 0, 3, 2 };
	std::vector<Texture> textures = {};

	m_mesh = Mesh(vertices, indices, textures);
}

void Laser::drawLaser(Shader& shader, Camera camera)
{
	shader.useProgram();
	glUniform3f(glGetUniformLocation(shader.m_id, "u_camera_positon"), camera.m_position.x, camera.m_position.y, camera.m_position.z);
	glUniform3f(glGetUniformLocation(shader.m_id, "u_camera_orientation"), camera.m_orientation.x, camera.m_orientation.y, camera.m_orientation.z);
	glUniformMatrix4fv(glGetUniformLocation(shader.m_id, "u_camera_view_matrix"), 1, GL_FALSE, glm::value_ptr(camera.m_view_matrix));
	m_mesh.drawMesh(shader, camera);
}
