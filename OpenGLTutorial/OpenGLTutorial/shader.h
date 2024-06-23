#pragma once
#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>

std::string getFileContents(const char* file_name);

/// <summary>
/// Shader's Class
/// </summary>
class Shader
{
public:
	GLuint m_id;
	Shader(const char* vertex_file, const char* fragment_file, const char* geometry_file = NULL);

	void useProgram();
	void deleteProgram();
	void checkErrors(unsigned int shader, const char* type, const char* file_name);
};