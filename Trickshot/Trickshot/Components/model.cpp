#include "model.h"

/// <summary>
/// Model Constructor
/// </summary>
/// <param name="file_path">Model File Path</param>

/// <summary>
/// Model Constructor
/// </summary>
/// <param name="file_path">Model File Path</param>
/// <param name="instance_number">No of. Instance (Default 1)</param>
/// <param name="instance_matrix">Instance Matrix's Vector (Need if No of. Instance > 1)</param>
Model::Model(const char* file_path, unsigned int instance_number, std::vector<glm::mat4> instance_matrix)
{
	std::string text = getFileContents(file_path);
	m_json = json::parse(text);

	m_file_path = file_path;
	m_data = getData();

	m_instance_number = instance_number;
	m_instance_matrix = instance_matrix;

	traverseNode(0);
}

/// <summary>
/// Draw Model
/// </summary>
/// <param name="shader">Shader's Program</param>
/// <param name="camera">Camera</param>
void Model::drawModel(Shader& shader, Camera& camera, glm::vec3 translation, glm::quat rotation, glm::vec3 scale)
{
	for (unsigned int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].Mesh::drawMesh(shader, camera, m_meshes_matrices[i], translation, rotation, scale);
	}
}

/// <summary>
/// Get Data from JSON accessor
/// </summary>
/// <returns>Data from JSON</returns>
std::vector<unsigned char> Model::getData()
{
	std::string bytes_text;
	std::string uri = m_json["buffers"][0]["uri"];

	std::string file_str = std::string(m_file_path);
	std::string file_directory = file_str.substr(0, file_str.find_last_of('/') + 1);
	bytes_text = getFileContents((file_directory + uri).c_str());

	std::vector<unsigned char> data(bytes_text.begin(), bytes_text.end());
	return data;
}

/// <summary>
/// Traverse Node between Meshes to collect all Meshes' matrices. (Recursive Function)
/// </summary>
/// <param name="next_node">id of next node</param>
/// <param name="matrix">matrix of next node</param>
void Model::traverseNode(unsigned int next_node, glm::mat4 matrix)
{
	json node = m_json["nodes"][next_node];

	//Translation
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	if (node.find("translation") != node.end())
	{
		float translation_values[3];
		for (unsigned int i = 0; i < node["translation"].size(); i++)
		{
			translation_values[i] = (node["translation"][i]);
		}
		translation = glm::make_vec3(translation_values);
	}

	//Rotation
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	if (node.find("rotation") != node.end())
	{
		// GLM have difference order from JSON accessor load
		float rotation_values[4] =
		{
			node["rotation"][3],
			node["rotation"][0],
			node["rotation"][1],
			node["rotation"][2]
		};
		rotation = glm::make_quat(rotation_values);
	}

	//Scale
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	if (node.find("scale") != node.end())
	{
		float scale_values[3];
		for (unsigned int i = 0; i < node["scale"].size(); i++)
		{
			scale_values[i] = (node["scale"][i]);
		}
		scale = glm::make_vec3(scale_values);
	}

	//Matrix
	glm::mat4 matrix_this_node = glm::mat4(1.0f);
	if (node.find("matrix") != node.end())
	{
		float matrix_values[16];
		for (unsigned int i = 0; i < node["matrix"].size(); i++)
		{
			matrix_values[i] = (node["matrix"][i]);
		}
		matrix_this_node = glm::make_mat4(matrix_values);
	}

	//Convert into Matrix
	glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), translation);
	glm::mat4 rotation_matrix = glm::mat4_cast(rotation);
	glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), scale);

	glm::mat4 matrix_next_node = matrix * matrix_this_node * translation_matrix * rotation_matrix * scale_matrix;

	// Check if the node contains a mesh and if it does load it
	if (node.find("mesh") != node.end())
	{
		m_meshes_translations.push_back(translation);
		m_meshes_rotations.push_back(rotation);
		m_meshes_scales.push_back(scale);
		m_meshes_matrices.push_back(matrix_next_node);
		loadMesh(node["mesh"]);
	}

	//Check if the node has children, and if it does, apply this function to them with the matNextNode
	//Recusrive for child
	if (node.find("children") != node.end())
	{
		for (unsigned int i = 0; i < node["children"].size(); i++)
			traverseNode(node["children"][i], matrix_next_node);
	}
}

/// <summary>
/// Get a vector of floats from JSON accessor
/// </summary>
/// <seealso href="https://github.com/KhronosGroup/glTF/tree/main/specification/2.0">
/// GLTF Github
/// </seealso>
/// <param name="accessor">JSON accessor</param>
/// <returns>A vector of floats</returns>
std::vector<float> Model::getFloats(json accessor)
{
	std::vector<float> floats_vector;

	// Buffer View Index: Point buffer view
	unsigned int buffer_view_index = accessor.value("bufferView", 1);
	// Count: How many float we have to get
	unsigned int count = accessor["count"];
	// Access Byte Offset: Byte Index to look at in data (the beginning of the buffer_view)
	unsigned int access_byte_offset = accessor.value("byteOffset", 0);
	// Type: How it should be group (e.g. Scalar)
	std::string type = accessor["type"];

	// Data needed
	json buffer_view = m_json["bufferViews"][buffer_view_index];
	// Buffer Offset (which maybe seperated by each locator).
	unsigned int byte_offset = buffer_view["byteOffset"];

	unsigned int number_per_vertice;
	
	if (type == "SCALAR")
	{
		number_per_vertice = 1;
	}
	else if (type == "VEC2")
	{
		number_per_vertice = 2;
	}
	else if (type == "VEC3")
	{
		number_per_vertice = 3;
	}
	else if (type == "VEC4")
	{
		number_per_vertice = 4;
	}
	else
	{
		throw std::invalid_argument("Type is invalid (not SCALAR, VEC2, VEC3, or VEC4)");
	}

	unsigned int beginning_of_data = byte_offset + access_byte_offset;
	unsigned int length_of_data = count * 4 * number_per_vertice;

	for (unsigned int i = beginning_of_data; i < beginning_of_data + length_of_data; i)
	{
		unsigned char bytes[] = { m_data[i++], m_data[i++], m_data[i++], m_data[i++] };
		float value;
		std::memcpy(&value, bytes, sizeof(float));
		floats_vector.push_back(value);
	}

	return floats_vector;
}

/// <summary>
/// Get a vector of indices from JSON accessor
/// </summary>
/// <param name="accessor">JSON accessor</param>
/// <returns>A vector of indices</returns>
std::vector<GLuint> Model::getIndices(json accessor)
{
	std::vector<GLuint> indices_vector;

	// Buffer View Index: Point buffer view
	unsigned int buffer_view_index = accessor.value("bufferView", 0);
	// Count: How many float we have to get
	unsigned int count = accessor["count"];
	// Access Byte Offset: Byte Index to look at in data (the beginning of the buffer_view)
	unsigned int access_byte_offset = accessor.value("byteOffset", 0);
	// Type: How it should be group (e.g. Scalar)
	unsigned int component_type = accessor["componentType"];

	// Data needed
	json buffer_view = m_json["bufferViews"][buffer_view_index];
	// Buffer Offset (which maybe seperated by each locator).
	unsigned int byte_offset = buffer_view["byteOffset"];

	unsigned int beginning_of_data = byte_offset + access_byte_offset;
	
	if (component_type == 5125) //Unsigned Integer
	{
		for (unsigned int i = beginning_of_data; i < byte_offset + access_byte_offset + count * 4; i)
		{
			unsigned char bytes[] = { m_data[i++], m_data[i++], m_data[i++], m_data[i++] };
			unsigned int value;
			std::memcpy(&value, bytes, sizeof(unsigned int));
			indices_vector.push_back((GLuint)value);
		}
	}
	else if (component_type == 5123) //Unsigned Short
	{
		for (unsigned int i = beginning_of_data; i < byte_offset + access_byte_offset + count * 2; i)
		{
			unsigned char bytes[] = { m_data[i++], m_data[i++] };
			unsigned short value;
			std::memcpy(&value, bytes, sizeof(unsigned short));
			indices_vector.push_back((GLuint)value);
		}
	}
	else if (component_type == 5122) //Short
	{
		for (unsigned int i = beginning_of_data; i < byte_offset + access_byte_offset + count * 2; i)
		{
			unsigned char bytes[] = { m_data[i++], m_data[i++] };
			short value;
			std::memcpy(&value, bytes, sizeof(short));
			indices_vector.push_back((GLuint)value);
		}
	}
	else
	{
		throw std::invalid_argument("Type is invalid (not 5125, 5123, 5122)");
	}

	return indices_vector;
}

/// <summary>
/// Get texture from JSON accessor
/// </summary>
/// <returns>a texture</returns>
std::vector<Texture> Model::getTextures()
{
	std::vector<Texture> textures;
	
	std::string file_str = std::string(m_file_path);
	std::string file_directory = file_str.substr(0, file_str.find_last_of('/') + 1);

	unsigned int unit = 0;
	for (unsigned int i = 0; i < m_json["images"].size(); i++)
	{
		std::string texture_path = m_json["images"][i]["uri"];
		bool skip = false;
		for (int unsigned j = 0; j < m_loaded_texture_name.size(); j++)
		{
			if (m_loaded_texture_name[j] == texture_path)
			{
				textures.push_back(m_loaded_texture[j]);
				skip = true;
				break;
			}
		}

		if(skip == false)
		{ 
			//note: keyword (e.g. "baseColor") depends on model.
			if (texture_path.find("baseColor") != std::string::npos || texture_path.find("diffuse") != std::string::npos)
			{
				Texture diffuse = Texture((file_directory + texture_path).c_str(), Texture::Category::DIFFUSE, m_loaded_texture.size());
				textures.push_back(diffuse);
				m_loaded_texture.push_back(diffuse);
				m_loaded_texture_name.push_back(texture_path);
			}
			else if (texture_path.find("metallicRoughness") != std::string::npos || texture_path.find("specular") != std::string::npos)
			{
				Texture specular = Texture((file_directory + texture_path).c_str(), Texture::Category::SPECULAR, m_loaded_texture.size());
				textures.push_back(specular);
				m_loaded_texture.push_back(specular);
				m_loaded_texture_name.push_back(texture_path);
			}
		}
	}

	return textures;
}

/// <summary>
/// Load meshes from JSON accessor
/// </summary>
/// <param name="mesh_index">index of current mesh</param>
void Model::loadMesh(unsigned int mesh_index)
{
	// Get all accessor indices
	unsigned int position_accessor_index = m_json["meshes"][mesh_index]["primitives"][0]["attributes"]["POSITION"];
	unsigned int normal_accessor_index = m_json["meshes"][mesh_index]["primitives"][0]["attributes"]["NORMAL"];
	unsigned int texture_coordinator_accessor_index = m_json["meshes"][mesh_index]["primitives"][0]["attributes"]["TEXCOORD_0"];
	unsigned int indice_accessor_index = m_json["meshes"][mesh_index]["primitives"][0]["indices"];
	
	// Use accessor indices to get all vertices components
	std::vector<float> position_vector = getFloats(m_json["accessors"][position_accessor_index]);
	std::vector<glm::vec3> positions = createGLMVec3FromFloats(position_vector);
	std::vector<float> normal_vector = getFloats(m_json["accessors"][normal_accessor_index]);
	std::vector<glm::vec3> normals = createGLMVec3FromFloats(normal_vector);
	std::vector<float> texture_coordinator_vector = getFloats(m_json["accessors"][texture_coordinator_accessor_index]);
	std::vector<glm::vec2> texture_coordinators = createGLMVec2FromFloats(texture_coordinator_vector);
	
	// Combine all the vertex components and also get the indices and textures
	std::vector<Vertex> vertices = createVertices(positions, normals, texture_coordinators);
	std::vector<GLuint> indices = getIndices(m_json["accessors"][indice_accessor_index]);
	std::vector<Texture> textures = getTextures();

	// Combine the vertices, indices, and textures into a mesh
	m_meshes.push_back(Mesh(vertices, indices, textures, m_instance_number, m_instance_matrix));

}

/// <summary>
/// Create Vertices from vector of each locations
/// </summary>
/// <param name="positions">vector of positions</param>
/// <param name="normals">vector of normal vectors</param>
/// <param name="texture_uvs">vector of texture coordinates</param>
/// <returns></returns>
std::vector<Vertex> Model::createVertices(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> texture_uvs)
{
	std::vector<Vertex> vertices;

	for (int i = 0; i < positions.size(); i++)
	{
		vertices.push_back(Vertex{ positions[i],normals[i],glm::vec3(1.0f, 1.0f, 1.0f), texture_uvs[i] });
	}

	return vertices;
}

/// <summary>
/// Create vector of glm::vec2 from vector of floats.
/// </summary>
/// <param name="floats_vector">vector of floats</param>
/// <returns>vector of glm::vec2</returns>
std::vector<glm::vec2> Model::createGLMVec2FromFloats(std::vector<float> floats_vector)
{
	std::vector<glm::vec2> vector;
	for (int i = 0; i < floats_vector.size(); i)
	{
		vector.push_back((glm::vec2(floats_vector[i++], floats_vector[i++])));
	}
	return vector;
}

/// <summary>
/// Create vector of glm::vec3 from vector of floats.
/// </summary>
/// <param name="floats_vector">vector of floats</param>
/// <returns>vector of glm::vec3</returns>
std::vector<glm::vec3> Model::createGLMVec3FromFloats(std::vector<float> floats_vector)
{
	std::vector<glm::vec3> vector;
	for (int i = 0; i < floats_vector.size(); i)
	{
		vector.push_back((glm::vec3(floats_vector[i++], floats_vector[i++], floats_vector[i++])));
	}
	return vector;
}

/// <summary>
/// Create vector of glm::vec4 from vector of floats.
/// </summary>
/// <param name="floats_vector">vector of floats</param>
/// <returns>vector of glm::vec4</returns>
std::vector<glm::vec4> Model::createGLMVec4FromFloats(std::vector<float> floats_vector)
{
	std::vector<glm::vec4> vector;
	for (int i = 0; i < floats_vector.size(); i)
	{
		vector.push_back((glm::vec4(floats_vector[i++], floats_vector[i++], floats_vector[i++], floats_vector[i++])));
	}
	return vector;
}