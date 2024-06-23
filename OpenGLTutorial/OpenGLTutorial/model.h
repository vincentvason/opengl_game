#pragma once
#include <json/json.hpp>
#include "mesh.h"

using json = nlohmann::json;

class Model
{
public:
	Model(const char* file_path, unsigned int instance_number = 1, std::vector<glm::mat4> instance_matrix = {});
	void drawModel(Shader& shader, Camera& camera, glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f), glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));

private:
	const char* m_file_path;
	std::vector<unsigned char> m_data;
	json m_json;
	unsigned int m_instance_number;
	std::vector<glm::mat4> m_instance_matrix;

	std::vector<std::string> m_loaded_texture_name;
	std::vector<Texture> m_loaded_texture;

	std::vector<Mesh> m_meshes;
	std::vector<glm::vec3> m_meshes_translations;
	std::vector<glm::quat> m_meshes_rotations;
	std::vector<glm::vec3> m_meshes_scales;
	std::vector<glm::mat4> m_meshes_matrices;

	std::vector<unsigned char> getData();
	void traverseNode(unsigned int next_node, glm::mat4 matrix = glm::mat4(1.0f));

	std::vector<float> getFloats(json accessor);
	std::vector<GLuint> getIndices(json accessor);
	std::vector<Texture> getTextures();

	void loadMesh(unsigned int mesh_index);


	std::vector<Vertex> createVertices(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> texture_uvs);

	std::vector<glm::vec2> createGLMVec2FromFloats(std::vector<float> float_vector);
	std::vector<glm::vec3> createGLMVec3FromFloats(std::vector<float> float_vector);
	std::vector<glm::vec4> createGLMVec4FromFloats(std::vector<float> float_vector);
};