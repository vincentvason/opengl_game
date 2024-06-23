#include "main.h"
#include "mesh.h"

const int HEIGHT = 800;
const int WIDTH = 800;

// Vertices coordinates
// Positions, Normal Vectors, Colors, Texture UVs
Vertex vertices_floor[] =
{
	Vertex{glm::vec3(-10.0f, 0.0f,  10.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f,  0.0f)},
	Vertex{glm::vec3(-10.0f, 0.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f,  10.0f)},
	Vertex{glm::vec3(10.0f,  0.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(10.0f, 10.0f)},
	Vertex{glm::vec3(10.0f,  0.0f,  10.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(10.0f, 0.0f)}
};

// Indices for vertices order
GLuint indices_floor[] =
{
	0, 1, 2,
	0, 2, 3
};

// Vertices coordinates
// Positions
Vertex vertices_light[] =
{
	Vertex{glm::vec3(-0.1f, -0.1f + 0.2f,  0.1f)},
	Vertex{glm::vec3(-0.1f, -0.1f + 0.2f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f + 0.2f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f + 0.2f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f + 0.2f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f + 0.2f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f + 0.2f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f + 0.2f,  0.1f)}
};

GLuint indices_light[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};

void mainMesh()
{
	// Initialize GLFW
	glfwInit();

	// Create a GLFWwindow object of 800 by 800 pixels.
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", NULL, NULL);

	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}

	// Introduce the window into the current context
	glfwMakeContextCurrent(window);

	// Load GLAD so it configures OpenGL
	gladLoadGL();

	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, WIDTH, HEIGHT);

	// Texture data
	Texture textures[]
	{
		Texture("floor_diffuse.png", Texture::DIFFUSE, 0),
		Texture("floor_specular.png", Texture::SPECULAR, 1)
	};

	// Generates Shader object using shaders default.vert and default.frag
	Shader shader_default("default_mesh.vert", "default_mesh.frag");
	// Store mesh data in vectors for the mesh
	std::vector <Vertex> vert(vertices_floor, vertices_floor + sizeof(vertices_floor) / sizeof(Vertex));
	std::vector <GLuint> ind(indices_floor, indices_floor + sizeof(indices_floor) / sizeof(GLuint));
	std::vector <Texture> tex(textures, textures + sizeof(textures) / sizeof(Texture));
	// Create floor mesh
	Mesh floor(vert, ind, tex);

	// Shader for light cube
	Shader shader_light("light.vert", "light.frag");
	// Store mesh data in vectors for the mesh
	std::vector <Vertex> vert_light(vertices_light, vertices_light + sizeof(vertices_light) / sizeof(Vertex));
	std::vector <GLuint> ind_light(indices_light, indices_light + sizeof(indices_light) / sizeof(GLuint));
	// Create light mesh
	Mesh light(vert_light, ind_light, tex);

	//Set Uniform Parameter
	glm::vec4 light_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 light_position = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 light_model = glm::mat4(1.0f);
	light_model = glm::translate(light_model, light_position);

	glm::vec3 floor_position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 floor_model = glm::mat4(1.0f);
	floor_model = glm::translate(floor_model, floor_position);

	shader_light.useProgram();
	glUniformMatrix4fv(glGetUniformLocation(shader_light.m_id, "u_light_model_matrix"), 1, GL_FALSE, glm::value_ptr(light_model));
	glUniform4f(glGetUniformLocation(shader_light.m_id, "u_light_color"), light_color.x, light_color.y, light_color.z, light_color.w);
	shader_default.useProgram();
	glUniformMatrix4fv(glGetUniformLocation(shader_default.m_id, "u_model_matrix"), 1, GL_FALSE, glm::value_ptr(floor_model));
	glUniform4f(glGetUniformLocation(shader_default.m_id, "u_light_color"), light_color.x, light_color.y, light_color.z, light_color.w);
	glUniform3f(glGetUniformLocation(shader_default.m_id, "u_light_position"), light_position.x, light_position.y, light_position.z);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Creates camera object
	Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f), 0.0005f, 2.5f);

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Specify the color of the background
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Handles camera inputs
		camera.handleInput(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.setReferenceMatrices(45.0f, 0.1f, 100.0f);


		// Draws different meshes
		floor.drawMesh(shader_default, camera);
		light.drawMesh(shader_light, camera);


		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Delete all the objects we've created
	shader_default.deleteProgram();
	shader_light.deleteProgram();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();

}