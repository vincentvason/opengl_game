#pragma once
#include "mesh.h"

class Laser {
public:
	Mesh m_mesh;

public:
	Laser(float weight = 0.1);
	void drawLaser(Shader& shader, Camera camera);
};