#pragma once
#include "mesh.h"

class Laser {
private:
	float m_weight;
	glm::vec3 m_src;
	glm::vec3 m_dst;

	bool m_collided;
	glm::vec3 m_next_intersect;
	glm::vec3 m_next_normal_vector;

	std::vector<Mesh> m_collision_meshes;

public:
	Mesh m_mesh;

public:
	Laser(std::vector<Mesh> collision_meshes, float weight = 0.004);
	void drawLaser(Shader& shader, Camera camera);
	void generateMeshStructure(Camera camera, glm::vec3 src, glm::vec3 dst);
	bool findNextLaserLine(glm::vec3 src, glm::vec3 dst, std::vector<Mesh> meshes);
	bool findIntersection(glm::vec3 src, glm::vec3 dst, std::vector<glm::vec3> planes);
};