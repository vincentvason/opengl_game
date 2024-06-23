#include "main.h"
#include "model.h"

const int HEIGHT = 800;
const int WIDTH = 800;

// Takes care of the information needed to draw the windows
const unsigned int boom_number = 100;
glm::vec3 boom_positions[boom_number];
float boom_rotations[boom_number];

// Takes care of drawing the windows in the right order
unsigned int boom_draw_orders[boom_number];
float boom_camera_distances[boom_number];

// Compare function
int compare(const void* a, const void* b)
{
	double diff = boom_camera_distances[*(int*)b] - boom_camera_distances[*(int*)a];
	return  (0 < diff) - (diff < 0);
}

void mainBlending()
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

	// Generates Shader object using shaders default.vert and default.frag
	Shader shader_default("default_model.vert", "default_model.frag");
	Shader shader_grass("default_model.vert", "grass.frag");
	Shader shader_boom("default_model.vert", "boom.frag");

	// Load Model
	Model ground("models/ground2/scene.gltf");
	Model grass("models/grass/scene.gltf");
	Model boom("models/boom/scene.gltf");

	// Set Uniform Parameter
	glm::vec4 light_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 light_position = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 light_model = glm::mat4(1.0f);
	light_model = glm::translate(light_model, light_position);

	shader_default.useProgram();
	glUniform4f(glGetUniformLocation(shader_default.m_id, "u_light_color"), light_color.x, light_color.y, light_color.z, light_color.w);
	glUniform3f(glGetUniformLocation(shader_default.m_id, "u_light_position"), light_position.x, light_position.y, light_position.z);
	shader_grass.useProgram();
	glUniform4f(glGetUniformLocation(shader_default.m_id, "u_light_color"), light_color.x, light_color.y, light_color.z, light_color.w);
	glUniform3f(glGetUniformLocation(shader_default.m_id, "u_light_position"), light_position.x, light_position.y, light_position.z);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Enables Cull Facing
	glEnable(GL_CULL_FACE);
	// Keeps front faces
	glCullFace(GL_FRONT);

	// Uses counter clock-wise standard
	glFrontFace(GL_CCW);
	// Configures the blending function
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Generates all windows
	for (unsigned int i = 0; i < boom_number; i++)
	{
		boom_positions[i] = glm::vec3
		(
			-15.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (15.0f - (-15.0f)))),
			1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (4.0f - 1.0f))),
			-15.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (15.0f - (-15.0f))))
		);
		boom_rotations[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.0f));
		boom_draw_orders[i] = i;
	}

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
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Updates and exports the camera matrix to the Vertex Shader
		camera.setReferenceMatrices(45.0f, 0.1f, 100.0f);

		// Draws different meshes
		ground.drawModel(shader_default, camera);

		// Disable cull face so that grass and windows have both faces
		glDisable(GL_CULL_FACE);
		grass.drawModel(shader_grass, camera);
		// Enable blending for windows
		glEnable(GL_BLEND);
		// Get distance from each window to the camera
		for (unsigned int i = 0; i < boom_number; i++)
		{
			boom_camera_distances[i] = glm::length(camera.m_position - boom_positions[i]);
		}
		// Sort windows by distance from camera
		qsort(boom_draw_orders, boom_number, sizeof(unsigned int), compare);
		// Draw windows
		for (unsigned int i = 0; i < boom_number; i++)
		{
			boom.drawModel(shader_boom, camera, boom_positions[boom_draw_orders[i]], glm::quat(1.0f, 0.0f, boom_rotations[boom_draw_orders[i]], 0.0f));
		}
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Delete all the objects we've created
	shader_default.deleteProgram();
	shader_grass.deleteProgram();
	shader_boom.deleteProgram();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();

}