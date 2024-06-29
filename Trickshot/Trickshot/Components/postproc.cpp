#include "postproc.h"

/// <summary>
/// Post-Processing VAO Buffer for Post-processing Screen
/// </summary>
/// <param name="width">Screen's Width</param>
/// <param name="height">Screen's Height</param>
PostProcessing::PostProcessing(unsigned int width, unsigned int height)
{
	float rectangle_vertices[] =
	{
		//  Coords   // texCoords
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};

	m_width = width;
	m_height = height;

	// Prepare framebuffer rectangle VBO and VAO
	unsigned int vao_rect, vbo_rect;
	glGenVertexArrays(1, &vao_rect);
	glGenBuffers(1, &vbo_rect);
	glBindVertexArray(vao_rect);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_rect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), rectangle_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	m_vao_id = vao_rect;
}

/// <summary>
/// Initialize MSAA Frame Buffer and Texture
/// </summary>
/// <param name="msaa_simple">Number of Sample</param>
void PostProcessing::initMSAA(unsigned int msaa_simple)
{
	// Create Frame Buffer Object
	unsigned int fbo_msaa_id;
	glGenFramebuffers(1, &fbo_msaa_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_msaa_id);

	// Create Framebuffer Texture
	unsigned int fbtex_msaa_id;
	glGenTextures(1, &fbtex_msaa_id);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fbtex_msaa_id);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaa_simple, GL_RGB16F, m_width, m_height, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, fbtex_msaa_id, 0);

	// Create Render Buffer Object
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, msaa_simple, GL_DEPTH24_STENCIL8, m_width, m_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	// Error checking framebuffer
	auto fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fbo_status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer error: " << fbo_status << std::endl;

	m_msaa_id = fbo_msaa_id;
	m_msaa_texture = fbtex_msaa_id;
}

/// <summary>
/// Initialize Gamma Frame Buffer and Texture
/// </summary>
/// <param name="gamma_value">Gamma's Value</param>
void PostProcessing::initGamma(float gamma_value)
{
	// Create Frame Buffer Object
	unsigned int fbo_post_id;
	glGenFramebuffers(1, &fbo_post_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_post_id);

	// Create Framebuffer Texture
	unsigned int fbtex_post_id;
	glGenTextures(1, &fbtex_post_id);
	glBindTexture(GL_TEXTURE_2D, fbtex_post_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbtex_post_id, 0);

	// Error checking framebuffer
	auto fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fbo_status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer error: " << fbo_status << std::endl;

	m_gamma_id = fbo_post_id;
	m_gamma_texture = fbtex_post_id;
	m_gamma = gamma_value;
}

/// <summary>
/// Delete Post-processing related to FBO
/// </summary>
void PostProcessing::deleteFBO()
{
	glDeleteFramebuffers(1, &m_msaa_id);
	glDeleteFramebuffers(1, &m_gamma_id);
}
