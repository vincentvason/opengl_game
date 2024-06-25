#include "mesh.h"


/// <summary>
/// Mesh's constructor
/// </summary>
/// <param name="vertices">Vector of Vertices</param>
/// <param name="indices">Vector of Indices</param>
/// <param name="textures">Vector of Textures</param>
/// <param name="instance_number">Number of Instance (Default 1)</param>
/// <param name="instance_matrix">Instance Matrix's Vector (need if No. of Instance > 1)</param>
Mesh::Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures, unsigned int instance_number, std::vector<glm::mat4> instance_matrix)
{
	m_vertices = vertices;
	m_indices = indices;
	m_textures = textures;
	m_instance_number = instance_number;

	m_vao.bindVertexArray();

	VBO vbo_instance(instance_matrix);
	VBO vbo(vertices);
	EBO ebo(indices);

	//Enable Attribute for Position, Normal Vector, Color, Texture
	m_vao.enableVertexAttribArray(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	m_vao.enableVertexAttribArray(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
	m_vao.enableVertexAttribArray(vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
	m_vao.enableVertexAttribArray(vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

	if (instance_number != 1)
	{
		// Can't link to a mat4 so you need to link four vec4s
		m_vao.enableVertexAttribArray(vbo_instance, 4, 4, GL_FLOAT, sizeof(glm::mat4), (void*)0);
		m_vao.enableVertexAttribArray(vbo_instance, 5, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(1 * sizeof(glm::vec4)));
		m_vao.enableVertexAttribArray(vbo_instance, 6, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		m_vao.enableVertexAttribArray(vbo_instance, 7, 4, GL_FLOAT, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
		// Makes it so the transform is only switched when drawing the next instance
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
	}

	//Unbind
	m_vao.unbindVertexArray();
	vbo.unbindBuffer();
	ebo.unbindBuffer();
}

/// <summary>
/// Draw Mesh
/// </summary>
/// <param name="shader">Shader's Program</param>
/// <param name="camera">Camera</param>
/// <param name="model_matrix">Model Matrix (from Model)</param>
/// <param name="translation">Translation Vector (from Model)</param>
/// <param name="rotation">Rotation Quaternion (from Model)</param>
/// <param name="scale">Scale Vector (from Model)</param>
void Mesh::drawMesh(Shader& shader, Camera& camera, glm::mat4 model_matrix, glm::vec3 translation, glm::quat rotation, glm::vec3 scale)
{
	shader.useProgram();

	for (unsigned int i = 1; i < m_textures.size(); i++)
	{
		std::string number_string, uniform_name;
		Texture::Category category = m_textures[i].m_category;

		if (category == Texture::DIFFUSE)
		{
			number_string = std::to_string(m_number_diffuse++);
			uniform_name = ("u_diffuse" + number_string);
		}
		else if (category == Texture::SPECULAR)
		{
			number_string = std::to_string(m_number_specular++);
			uniform_name = ("u_specular" + number_string);
		}
		else if (category == Texture::NORMAL)
		{
			number_string = std::to_string(m_number_normal++);
			uniform_name = ("u_normal" + number_string);
		}
		else if (category == Texture::DISPLACEMENT)
		{
			number_string = std::to_string(m_number_displacement++);
			uniform_name = ("u_displacement" + number_string);
		}

		m_textures[i].bindTexture();
		glUniform1i(glGetUniformLocation(shader.m_id, uniform_name.c_str()), i);
	}

	shader.useProgram();

	std::string number_string, uniform_name;
	Texture::Category category = m_textures[0].m_category;

	if (category == Texture::DIFFUSE)
	{
		number_string = std::to_string(m_number_diffuse++);
		uniform_name = ("u_diffuse" + number_string);
	}
	else if (category == Texture::SPECULAR)
	{
		number_string = std::to_string(m_number_specular++);
		uniform_name = ("u_specular" + number_string);
	}
	else if (category == Texture::NORMAL)
	{
		number_string = std::to_string(m_number_normal++);
		uniform_name = ("u_normal" + number_string);
	}
	else if (category == Texture::DISPLACEMENT)
	{
		number_string = std::to_string(m_number_displacement++);
		uniform_name = ("u_displacement" + number_string);
	}

	m_textures[0].initTextureSlot(shader, uniform_name.c_str(), 0);
	m_textures[0].bindTexture();

	m_vao.bindVertexArray();

	//Send Uniform Camera-related to Shader
	glUniform3f(glGetUniformLocation(shader.m_id, "u_camera_position"), camera.m_position.x, camera.m_position.y, camera.m_position.z);
	camera.updateReferenceMatricesToShader(shader, "u_camera_matrix");

	if (m_instance_number == 1)
	{
		//Initialize Matrices
		glm::mat4 translation_matrix = glm::mat4(1.0f);
		glm::mat4 rotation_matrix = glm::mat4(1.0f);
		glm::mat4 scale_matrix = glm::mat4(1.0f);

		translation_matrix = glm::translate(glm::mat4(1.0f), translation);
		rotation_matrix = glm::mat4_cast(rotation);
		scale_matrix = glm::scale(glm::mat4(1.0f), scale);

		//Send Uniform Matrices to Shader
		glUniformMatrix4fv(glGetUniformLocation(shader.m_id, "u_translation_matrix"), 1, GL_FALSE, glm::value_ptr(translation_matrix));
		glUniformMatrix4fv(glGetUniformLocation(shader.m_id, "u_rotation_matrix"), 1, GL_FALSE, glm::value_ptr(rotation_matrix));
		glUniformMatrix4fv(glGetUniformLocation(shader.m_id, "u_scale_matrix"), 1, GL_FALSE, glm::value_ptr(scale_matrix));
		glUniformMatrix4fv(glGetUniformLocation(shader.m_id, "u_model_matrix"), 1, GL_FALSE, glm::value_ptr(model_matrix));

		//Draw
		glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	}
	else
	{
		//Draw
		glDrawElementsInstanced(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0, m_instance_number);
	}
}