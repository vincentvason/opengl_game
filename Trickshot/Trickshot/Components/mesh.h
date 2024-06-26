#pragma once
#include <string>
#include <vector>
#include "vao.h"
#include "ebo.h"
#include "camera.h"
#include "texture.h"

/// <summary>
/// Mesh Class
/// </summary>
class Mesh
{
private:
	unsigned int m_number_diffuse = 0;
	unsigned int m_number_specular = 0;
	unsigned int m_number_normal = 0;
	unsigned int m_number_displacement = 0;

public:
	std::vector <Vertex> m_vertices;
	std::vector <GLuint> m_indices;
	std::vector <Texture> m_textures;

	VAO m_vao;
	unsigned int m_instance_number;

public:
	Mesh() {};
	Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures, unsigned int instance_number = 1, std::vector<glm::mat4> instance_matrix = {});
	void drawMesh(Shader& shader, Camera& camera, glm::mat4 model_matrix = glm::mat4(1.0f), glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f), glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));

};
