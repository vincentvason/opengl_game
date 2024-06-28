#include "stage.h"

Stage::Stage(glm::vec3 room_size, std::vector<Texture> texture_top, std::vector<Texture> textures_side, std::vector<Texture> textures_bottom, glm::vec3 camera_pos, float camera_thresold)
{
	float limx = room_size[0];
	float limy = room_size[1];
	float limz = room_size[2];
	
	std::vector<Vertex> vertices_side1 =
	{
		//Wall
		Vertex{glm::vec3(0.0f, 0.0f, limz), glm::vec3(0.0f, 0.0f, limz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(limx, 0.0f, limz), glm::vec3(0.0f, 0.0f, limz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, limx)},
		Vertex{glm::vec3(limx, limy, limz), glm::vec3(0.0f, 0.0f, limz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(limy, limx)},
		Vertex{glm::vec3(0.0f, limy, limz), glm::vec3(0.0f, 0.0f, limz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(limy, 0.0f)},
	};

	std::vector<Vertex> vertices_side2 =
	{
		//Wall
		Vertex{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, limz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(limx, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, limz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, limx)},
		Vertex{glm::vec3(limx, limy, 0.0f), glm::vec3(0.0f, 0.0f, limz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(limy, limx)},
		Vertex{glm::vec3(0.0f, limy, 0.0f), glm::vec3(0.0f, 0.0f, limz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(limy, 0.0f)},
	};

	std::vector<Vertex> vertices_side3 =
	{
		//Wall
		Vertex{glm::vec3(limx, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, limz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(limy, limz)},
		Vertex{glm::vec3(limx, 0.0f, limz), glm::vec3(0.0f, 0.0f, limz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(limy, 0.0f)},
		Vertex{glm::vec3(limx, limy, limz), glm::vec3(0.0f, 0.0f, limz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(limx, limy, 0.0f), glm::vec3(0.0f, 0.0f, limz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, limz)},
	};

	std::vector<Vertex> vertices_side4 =
	{
		//Wall
		Vertex{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, limz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(limy, limz)},
		Vertex{glm::vec3(0.0f, 0.0f, limz), glm::vec3(0.0f, 0.0f, limz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(limy, 0.0f)},
		Vertex{glm::vec3(0.0f, limy, limz), glm::vec3(0.0f, 0.0f, limz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(0.0f, limy, 0.0f), glm::vec3(0.0f, 0.0f, limz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, limz)}
	};

	std::vector<Vertex> vertices_top =
	{
		//Ceiling
		Vertex{ glm::vec3(0.0f, limy, 0.0f), glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
		Vertex{ glm::vec3(0.0f, limy, limz), glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(limz, 0.0f) },
		Vertex{ glm::vec3(limx, limy, limz), glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(limz, limx) },
		Vertex{ glm::vec3(limx, limy, 0.0f), glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, limx) },
	};

	std::vector<Vertex> vertices_bottom =
	{
		//Floor
		Vertex{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(0.0f, 0.0f, limz), glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(limz, 0.0f)},
		Vertex{glm::vec3(limx, 0.0f, limz), glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(limz, limx)},
		Vertex{glm::vec3(limx, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 4.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, limx)},
	};

	std::vector<GLuint> indices_ccw = { 0, 1, 2, 0, 2, 3 };
	std::vector<GLuint> indices_cw = { 0, 2, 1, 0, 3, 2, };

	m_meshes.push_back(Mesh(vertices_side1, indices_ccw, textures_side));
	m_meshes.push_back(Mesh(vertices_side2, indices_cw, textures_side));
	m_meshes.push_back(Mesh(vertices_side3, indices_cw, textures_side));
	m_meshes.push_back(Mesh(vertices_side4, indices_ccw, textures_side));
	m_meshes.push_back(Mesh(vertices_top, indices_ccw, texture_top));
	m_meshes.push_back(Mesh(vertices_bottom, indices_cw, textures_bottom));

	

	m_camera_start_position = room_size * camera_pos;
	
	for (int i = 0; i < 3; i++)
	{
		m_camera_start_position[i] = std::min(m_camera_start_position[i], room_size[i] - camera_thresold);
		m_camera_start_position[i] = std::max(m_camera_start_position[i], camera_thresold);
	}

	m_room_size = room_size;
	m_camera_threshold = camera_thresold;
}

void Stage::drawStage(Shader& shader, Camera camera)
{
	for(int i = 0; i < m_meshes.size(); i++)
	{
		m_meshes[i].drawMesh(shader, camera);
	}	
}


