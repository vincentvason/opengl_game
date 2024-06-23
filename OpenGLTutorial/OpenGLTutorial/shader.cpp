#include "shader.h"

/// <summary>
/// Get string from file contents
/// </summary>
/// <param name="filename">File's direction</param>
/// <returns>String of Contents</returns>
std::string getFileContents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw(errno);
}

/// <summary>
/// Shader's Constructor
/// </summary>
/// <param name="vertex_file">File Directory of Vertex's File (.vert)</param>
/// <param name="fragment_file">File Directory of Fragment's File (.flag)</param>
/// <param name="geometry_file">File Directory of Gemotery's File (.flag)</param>
Shader::Shader(const char* vertex_file, const char* fragment_file, const char* geometry_file)
{
	//Get Vertex and Fragment Shader Code
	std::string vertex_code_str = getFileContents(vertex_file);
	std::string fragment_code_str = getFileContents(fragment_file);

	const char* vertex_code_c_str = vertex_code_str.c_str();
	const char* fragment_code_c_str = fragment_code_str.c_str();

	//Create Vertex Shader, Attach Shader Source and Compile to machine code.
	GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader_id, 1, &vertex_code_c_str, NULL);
	glCompileShader(vertex_shader_id);
	checkErrors(vertex_shader_id, "VERTEX", vertex_file);

	//Create Fragment Shader, Attach Shader Source and Compile to machine code.
	GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader_id, 1, &fragment_code_c_str, NULL);
	glCompileShader(fragment_shader_id);
	checkErrors(fragment_shader_id, "FRAGMENT", fragment_file);

	//Create Geometry Shader, Attach Shader Source and Compile to machine code.
	GLuint geometry_shader_id;
	if (geometry_file != NULL)
	{
		std::string geometry_code_str = getFileContents(geometry_file);
		const char* geometry_code_c_str = geometry_code_str.c_str();

		geometry_shader_id = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry_shader_id, 1, &geometry_code_c_str, NULL);
		glCompileShader(geometry_shader_id);
		checkErrors(geometry_shader_id, "GEOMETRY", geometry_file);
	}

	//Create Shader Program, Attach shaders, then link them.
	m_id = glCreateProgram();
	glAttachShader(m_id, vertex_shader_id);
	glAttachShader(m_id, fragment_shader_id);
	if (geometry_file != NULL)
	{
		glAttachShader(m_id, geometry_shader_id);
	}
	glLinkProgram(m_id);
	checkErrors(m_id, "PROGRAM", vertex_file);

	//Delete shader that already link to program.
	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);
	if (geometry_file != NULL)
	{
		glDeleteShader(geometry_shader_id);
	}
}

/// <summary>
/// Use Shader Program
/// </summary>
void Shader::useProgram()
{
	glUseProgram(m_id);
}

/// <summary>
/// Delete Shader Program
/// </summary>
void Shader::deleteProgram()
{
	glDeleteProgram(m_id);
}

/// <summary> 
/// Check and print error if shader program is failed to compile 
/// </summary> 
/// <seealso href="https://www.khronos.org/opengl/wiki/Shader_Compilation"> 
/// Shader Compilation
/// </seealso>
/// <param name="shader">shader program</param>
/// <param name="type">type of shader program</param>
/// <param name="type">shader's file name</param>
void Shader::checkErrors(unsigned int shader, const char* type, const char* file_name)
{
	// Stores status of compilation
	GLint hasCompiled;
	// Character array to store error message in
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR " << file_name << " for:" << type << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR " << file_name << " for:" "\n" << infoLog << std::endl;
		}
	}
}
