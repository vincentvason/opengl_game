#include "laser.h"

Laser::Laser(std::vector<Mesh> collision_meshes, float weight)
{
	m_weight = weight;
	m_collision_meshes = collision_meshes;
}

void Laser::drawLaser(Shader& shader, Camera camera)
{
	m_src = glm::vec3(-0.04f, 0.02f, 0.0f);
	m_dst = glm::vec3(0.0f, 0.0f, 16.0f);
	
	generateMeshStructure(camera, m_src, m_dst);
	shader.useProgram();
	m_mesh.drawMesh(shader, camera);

	m_src = glm::inverse(camera.m_view_matrix) * (glm::vec4(m_src, -1.0f) * -1.0f);
	m_dst = glm::inverse(camera.m_view_matrix) * (glm::vec4(m_dst, -1.0f) * -1.0f);
	
	
	printf("\033[H\033[J");
	findNextLaserLine(m_src, m_dst, m_collision_meshes);
	printf("Src: %.2f, %.2f, %.2f\n", m_src.x, m_src.y, m_src.z);
	printf("Dst: %.2f, %.2f, %.2f\n", m_dst.x, m_dst.y, m_dst.z);
	printf("Int: %.2f, %.2f, %.2f\n", m_next_intersect.x, m_next_intersect.y, m_next_intersect.z);
	printf("Norm: %.2f, %.2f, %.2f\n", m_next_normal_vector.x, m_next_normal_vector.y, m_next_normal_vector.z);
	printf("\033[%d;%dH", 0, 0);

}

void Laser::generateMeshStructure(Camera camera, glm::vec3 src, glm::vec3 dst)
{
	std::vector<glm::vec4> points =
	{
		glm::vec4(src.x - m_weight / 2, src.y, src.z, -1.0),
		glm::vec4(dst.x - m_weight / 2, dst.y, dst.z, -1.0),
		glm::vec4(dst.x + m_weight / 2, dst.y, dst.z, -1.0),
		glm::vec4(src.x + m_weight / 2, src.y, src.z, -1.0)
	};

	for (int i = 0; i < 4; i++)
	{
		points[i] = glm::inverse(camera.m_view_matrix) * (points[i] * -1.0f);
	}

	std::vector<Vertex> vertices =
	{
		Vertex{glm::vec3(points[0]), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(points[1]), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(points[2]), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(points[3]), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
	};
	
	std::vector<GLuint> indices = { 0, 2, 1, 0, 3, 2 };
	std::vector<Texture> textures = {};

	m_mesh = Mesh(vertices, indices, textures);
}

bool Laser::findNextLaserLine(glm::vec3 src, glm::vec3 dst, std::vector<Mesh> meshes)
{
	for (int m = 0; m < meshes.size(); m++)
	{
		for (int t = 0; t < meshes[m].m_indices.size() / 3; t++)
		{
			std::vector<glm::vec3> triangle =
			{
				meshes[m].m_vertices[meshes[m].m_indices[t * 3]].position,
				meshes[m].m_vertices[meshes[m].m_indices[t * 3 + 1]].position,
				meshes[m].m_vertices[meshes[m].m_indices[t * 3 + 2]].position
			};

			if (findIntersection(src, dst, triangle) == true)
			{
				printf("T1: %.2f, %.2f, %.2f\n", triangle[0].x, triangle[0].y, triangle[0].z);
				printf("T2: %.2f, %.2f, %.2f\n", triangle[1].x, triangle[1].y, triangle[1].z);
				printf("T3: %.2f, %.2f, %.2f\n", triangle[2].x, triangle[2].y, triangle[2].z);
				m_collided = true;
				return true;
			}
		}
	}

	printf("False Collided\n");
	m_collided = false;
	return false;
}

bool Laser::findIntersection(glm::vec3 src, glm::vec3 dst, std::vector<glm::vec3> planes)
{
	//Plane Normal Vector
	glm::vec3 plane_normal_vector = glm::normalize(glm::cross(planes[1] - planes[0], planes[2] - planes[0]));
	
	//Find Distance to a plane
	float dist1 = glm::dot(src - planes[0], plane_normal_vector);
	float dist2 = glm::dot(dst - planes[0], plane_normal_vector);
	glm::vec3 intersect_point;
	
	//if there is no intersection or parallel, it's false
	if ((dist1 * dist2) >= 0.0f || dist1 == dist2)
	{
		return false;
	}
	else
	{
		// Find point on the line that intersects with the plane
		intersect_point = src + (dst - src) * (-dist1 / (dist2 - dist1));
	}

	// Find if the interesection point lies inside the triangle by testing it against all edges
	glm::vec3 test_vector;
	test_vector = glm::cross(plane_normal_vector, planes[1] - planes[0]);
	if (glm::dot(test_vector, intersect_point - planes[0]) < 0.0f)
	{
		return false;
	}

	test_vector = glm::cross(plane_normal_vector, planes[2] - planes[1]);
	if (glm::dot(test_vector, intersect_point - planes[1]) < 0.0f)
	{
		return false;
	}

	test_vector = glm::cross(plane_normal_vector, planes[0] - planes[2]);
	if (glm::dot(test_vector, intersect_point - planes[2]) < 0.0f)
	{
		return false;
	}
	
	m_next_normal_vector = plane_normal_vector;
	m_next_intersect = intersect_point;
	return true;
}


