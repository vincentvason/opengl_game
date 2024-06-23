#include "main.h"
#include "model.h"

const int HEIGHT = 800;
const int WIDTH = 800;

void mainSkybox()
{
	float vertices_skybox[] =
	{
		//   Coordinates
		-1.0f, -1.0f,  1.0f,//        7--------6
		 1.0f, -1.0f,  1.0f,//       /|       /|
		 1.0f, -1.0f, -1.0f,//      4--------5 |
		-1.0f, -1.0f, -1.0f,//      | |      | |
		-1.0f,  1.0f,  1.0f,//      | 3------|-2
		 1.0f,  1.0f,  1.0f,//      |/       |/
		 1.0f,  1.0f, -1.0f,//      0--------1
		-1.0f,  1.0f, -1.0f
	};

	unsigned int indices_skybox[] =
	{
		// Right
		1, 2, 6,
		6, 5, 1,
		// Left
		0, 4, 7,
		7, 3, 0,
		// Top
		4, 5, 6,
		6, 7, 4,
		// Bottom
		0, 3, 2,
		2, 1, 0,
		// Back
		0, 1, 5,
		5, 4, 0,
		// Front
		3, 7, 6,
		6, 2, 3
	};

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

	// Generates Shader object using shaders default.vert and default.frag
	Shader shader_default("default_model.vert", "default_model.frag");
	Shader shader_skybox("skybox.vert", "skybox.frag");

	// Load Model
	Model airplane("models/airplane/scene.gltf");

	// Set Uniform Parameter
	glm::vec4 light_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 light_position = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 light_model = glm::mat4(1.0f);
	light_model = glm::translate(light_model, light_position);

	shader_default.useProgram();
	glUniform4f(glGetUniformLocation(shader_default.m_id, "u_light_color"), light_color.x, light_color.y, light_color.z, light_color.w);
	glUniform3f(glGetUniformLocation(shader_default.m_id, "u_light_position"), light_position.x, light_position.y, light_position.z);
	shader_skybox.useProgram();
	glUniform1i(glGetUniformLocation(shader_skybox.m_id, "u_skybox_texture"), 0);

	// Create VAO, VBO, and EBO for the skybox
	unsigned int vao_skybox_id, vbo_skybox_id, ebo_skybox_id;
	glGenVertexArrays(1, &vao_skybox_id);
	glGenBuffers(1, &vbo_skybox_id);
	glGenBuffers(1, &ebo_skybox_id);
	glBindVertexArray(vao_skybox_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_skybox_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_skybox), &vertices_skybox, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_skybox_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices_skybox), &indices_skybox, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	std::string cubemap_texture[6] =
	{
		"skybox/sky/right.jpg","skybox/sky/left.jpg","skybox/sky/top.jpg","skybox/sky/bottom.jpg","skybox/sky/front.jpg","skybox/sky/back.jpg"
	};

	// Creates the cubemap texture object
	unsigned int cubemap_tex;
	glGenTextures(1, &cubemap_tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_tex);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// This might help with seams on some systems
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Cycles through all the textures and attaches them to the cubemap object
	// Order in Cube Map is not the same with normal texture 2D
	stbi_set_flip_vertically_on_load(false);
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nr_channels;
		unsigned char* data = stbi_load(cubemap_texture[i].c_str(), &width, &height, &nr_channels, 0);

		// POSITIVE_X, NEGATIVE_X, POSITIVE_Y, NEGATIVE_Y, POSITIVE_Z, NEGATIVE_Z
		// Right, Left, Top, Bottom, Front, Back (it's left-handed, different from normal textures)
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << cubemap_texture[i] << std::endl;
			stbi_image_free(data);
		}
	}

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Enables Cull Facing
	glEnable(GL_CULL_FACE);
	// Keeps front faces
	glCullFace(GL_FRONT);
	// Uses counter clock-wise standard
	glFrontFace(GL_CCW);

	// Creates camera object
	Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));

	// Variables to create periodic event for FPS displaying
	double time_prev = 0.0;
	double time_crnt = 0.0;
	double time_diff;
	// Keeps track of the amount of frames in timeDiff
	unsigned int fps_counter = 0;

	// Use this to disable VSync (not advized)
	//glfwSwapInterval(0);

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Updates counter and times
		time_crnt = glfwGetTime();
		time_diff = time_crnt - time_prev;
		fps_counter++;

		if (time_diff >= 1.0 / 30.0)
		{
			// Creates new title
			std::string FPS = std::to_string((1.0 / time_diff) * fps_counter);
			std::string ms = std::to_string((time_diff / fps_counter) * 1000);
			std::string newTitle = "OpenGL [" + FPS + "FPS / " + ms + "ms]";
			glfwSetWindowTitle(window, newTitle.c_str());

			// Resets times and counter
			time_prev = time_crnt;
			fps_counter = 0;

			// Handles camera inputs - Use this if you have disabled VSync
			camera.handleInput(window);
		}

		// Specify the color of the background
		glClearColor(0.03f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Updates and exports the camera matrix to the Vertex Shader
		camera.setReferenceMatrices(45.0f, 0.1f, 100.0f);

		// Draw the normal model
		airplane.drawModel(shader_default, camera);

		// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
		glDepthFunc(GL_LEQUAL);

		shader_skybox.useProgram();
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		// We make the mat4 into a mat3 and then a mat4 again in order to get rid of the last row and column
		// The last row and column affect the translation of the skybox (which we don't want to affect)
		view = glm::mat4(glm::mat3(glm::lookAt(camera.m_position, camera.m_position + camera.m_orientation, camera.m_up)));
		projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
		glUniformMatrix4fv(glGetUniformLocation(shader_skybox.m_id , "u_view_matrix"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader_skybox.m_id, "u_projection_matrix"), 1, GL_FALSE, glm::value_ptr(projection));

		// Draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
		// where an object is present (a depth of 1.0f will always fail against any object's depth value)
		glBindVertexArray(vao_skybox_id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_tex);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Switch back to the normal depth function
		glDepthFunc(GL_LESS);

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Delete all the objects we've created
	shader_default.deleteProgram();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();

}