#include "texture.h"

/// <summary>
/// Texture Constructor
/// </summary>
/// <seealso href="https://registry.khronos.org/OpenGL-Refpages/gl4/">
/// OpenGL Reference
/// </seealso>
/// <param name="image_path">Image's Name</param>
/// <param name="texture_category">Category (see enum)</param>
/// <param name="slot_number">No. of Slot</param>
/// <param name="render_type">GL_NEAREST (sharp) or GL_LINEAR (smoothen)</param>
Texture::Texture(const char* image_path, Category texture_category, GLuint slot_number, GLenum render_type)
{
	//Assign Texture Category + Slot Number
	m_category = texture_category;

	//Texture
	int width_img, height_img, num_col_ch;
	//Convert STB Order to OpenGL Order
	stbi_set_flip_vertically_on_load(true);
	unsigned char* bytes = stbi_load(image_path, &width_img, &height_img, &num_col_ch, 0);

	//Generate Open GL and assign texture to texture unit
	glGenTextures(1, &m_id);
	glActiveTexture(GL_TEXTURE0 + slot_number);
	m_slot_number = slot_number;
	glBindTexture(GL_TEXTURE_2D, m_id);

	if (render_type == GL_NEAREST)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else if (render_type == GL_LINEAR)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		throw std::invalid_argument("Automatic Render Type Recognition Failed (Must be GL_NEAREST or GL_LINEAR)");
	}
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Extra lines in case, choosing to use GL_CLAMP_TO_BORDER
	//float flatColor[] = { 0.7f, 0.5f, 0.3f, 1.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

	//GL_SRGB_ALPHA (4 channels) and GL_SRGB (3 channels or less) are used if do gamma correction, otherwise use GL_RGBA;

	// Assigns the image to the OpenGL Texture object + Generates MipMaps
	if (texture_category == Texture::NORMAL && num_col_ch == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_img, height_img, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	}
	else if (texture_category == Texture::NORMAL && num_col_ch == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_img, height_img, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
	}
	else if (texture_category == Texture::DISPLACEMENT)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width_img, height_img, 0, GL_RED, GL_UNSIGNED_BYTE, bytes);
	}
	else if (num_col_ch == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width_img, height_img, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
	}
	else if (num_col_ch == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width_img, height_img, 0, GL_RGB, GL_UNSIGNED_BYTE, bytes);
	}
	else if (num_col_ch == 2)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width_img, height_img, 0, GL_RG, GL_UNSIGNED_BYTE, bytes);
	}
	else if (num_col_ch == 1)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width_img, height_img, 0, GL_RED, GL_UNSIGNED_BYTE, bytes);
	}
	else
	{
		throw std::invalid_argument("Automatic Texture Type Recognition Failed (num_col_ch must <= 4)");
	}


	glGenerateMipmap(GL_TEXTURE_2D);

	//Free and Unbind
	stbi_image_free(bytes);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/// <summary>
/// Init Texture's Uniform Unit
/// </summary>
/// <param name="shader">Shader Program</param>
/// <param name="uniform">Unifrom variable name (see .frag)</param>
/// <param name="slot_number">No. of Slot</param>
void Texture::initTextureSlot(Shader& shader, const char* uniform, GLuint slot_number)
{
	GLuint tex_uniform_location = glGetUniformLocation(shader.m_id, uniform);
	shader.useProgram();
	glUniform1i(tex_uniform_location, slot_number);
}

/// <summary>
/// Bind Texture
/// </summary>
void Texture::bindTexture()
{
	glActiveTexture(GL_TEXTURE0 + m_slot_number);
	glBindTexture(GL_TEXTURE_2D, m_id);
}

/// <summary>
/// Unbind Texture
/// </summary>
void Texture::unbindTexture()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

/// <summary>
/// Delete Texture
/// </summary>
void Texture::deleteTexture()
{
	glDeleteTextures(1, &m_id);
}








