#pragma once
#include "mesh.h"

/// <summary>
/// Post Processing Class
/// </summary>
class PostProcessing
{
private:
	unsigned int m_width;
	unsigned int m_height;

public:
	unsigned int m_vao_id = 0;

	unsigned int m_msaa_id = 0;
	unsigned int m_msaa_texture = 0;

	float m_gamma = 0;
	unsigned int m_gamma_id = 0;
	unsigned int m_gamma_texture = 0;

	PostProcessing(unsigned int width, unsigned int height);
	void initMSAA(unsigned int msaa_sample);
	void initGamma(float gamma_value);
	void deleteFBO();

};