#pragma once
#include<glad/glad.h>
#include<stb/stb_image.h>
#include"shader.h"

class Texture
{
public:
	enum Category
	{
		DIFFUSE,
		SPECULAR,
		NORMAL,
		DISPLACEMENT
	};

	GLuint m_id;
	Category m_category;
	GLenum m_slot_number;

	Texture(const char* image_path, Category texture_category, GLuint slot_number, GLenum render_type = GL_NEAREST);

	void initTextureSlot(Shader& shader, const char* uniform, GLuint slot_number);
	void bindTexture();
	void unbindTexture();
	void deleteTexture();

};