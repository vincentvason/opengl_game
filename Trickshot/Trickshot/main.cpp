#include "Components/model.h"
#include "Components/postproc.h"
#include "Components/stage.h"

void main()
{
	const unsigned int WIDTH = 1024;
	const unsigned int HEIGHT = 768;

	// Number of MSAA_SAMPLES per pixel for MSAA
	const unsigned int MSAA_SAMPLES = 8;

	// Controls the gamma function
	const float GAMMA = 2.2f;

	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Only use this if you don't have a framebuffer
	//glfwWindowHint(GLFW_SAMPLES, MSAA_SAMPLES);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
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

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, WIDTH, HEIGHT);

	// Generates shaders
	Shader shader_default("Shaders/default.vert", "Shaders/default.frag", "Shaders/default.geom");
	Shader shader_framebuffer("Shaders/framebuffer.vert", "Shaders/framebuffer.frag");

	// Enables Options Buffer
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

	// Use this to disable VSync (not recommended)
	//glfwSwapInterval(0);

	// Load things
	std::vector<Texture> textures_ceiling =
	{
		Texture("Resources/Ceiling_basecolor.jpg", Texture::DIFFUSE, 0),
		Texture("Resources/Ceiling_ambientOcclusion.jpg", Texture::SPECULAR, 1),
		Texture("Resources/Ceiling_normal.jpg", Texture::NORMAL, 2),
		Texture("Resources/Ceiling_height.png", Texture::DISPLACEMENT, 3)
	};
	std::vector<Texture> textures_wall =
	{
		Texture("Resources/Wall_basecolor.jpg", Texture::DIFFUSE, 0),
		Texture("Resources/Wall_ambientOcclusion.jpg", Texture::SPECULAR, 1),
		Texture("Resources/Wall_normal.jpg", Texture::NORMAL, 2),
		Texture("Resources/Wall_height.png", Texture::DISPLACEMENT, 3)
	};
	std::vector<Texture> textures_floor =
	{
		Texture("Resources/Floor_BaseColor.jpg", Texture::DIFFUSE, 0),
		Texture("Resources/Floor_AmbientOcclusion.jpg", Texture::SPECULAR, 1),
		Texture("Resources/Floor_Normal.jpg", Texture::NORMAL, 2),
		Texture("Resources/Floor_Height.jpg", Texture::DISPLACEMENT, 3)
	};

	// Plane with the texture
	Stage stage(glm::vec3(8.0f, 4.0f, 8.0f), textures_ceiling, textures_wall, textures_floor);

	// Creates camera object
	Camera camera(WIDTH, HEIGHT, stage.m_camera_start_position, 0.005f);

	// Take care of all the light related things
	glm::vec4 light_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 light_position = glm::vec3(4.0f, 3.2f, 4.0f);

	shader_default.useProgram();
	glUniform4f(glGetUniformLocation(shader_default.m_id, "u_light_color"), light_color.x, light_color.y, light_color.z, light_color.w);
	glUniform3f(glGetUniformLocation(shader_default.m_id, "u_light_position"), light_position.x, light_position.y, light_position.z);
	shader_framebuffer.useProgram();
	glUniform1i(glGetUniformLocation(shader_framebuffer.m_id, "u_screen_texture"), 0);
	glUniform1f(glGetUniformLocation(shader_framebuffer.m_id, "u_gamma"), GAMMA);

	// Post-Processing
	PostProcessing postproc(WIDTH, HEIGHT);
	postproc.initMSAA(MSAA_SAMPLES);
	postproc.initGamma(GAMMA);

	// Variables to create periodic event for FPS displaying
	double time_prev = 0.0;
	double time_crnt = 0.0;
	double time_diff;
	// Keeps track of the amount of frames in time_diff
	unsigned int counter = 0;

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// Updates counter and times
		time_crnt = glfwGetTime();
		time_diff = time_crnt - time_prev;
		counter++;

		if (time_diff >= 1.0 / 30.0)
		{
			// Creates new title
			std::string fps = std::to_string((1.0 / time_diff) * counter);
			std::string ms = std::to_string((time_diff / counter) * 1000);
			std::string newTitle = "OpenGL [" + fps + "FPS / " + ms + "ms]";
			glfwSetWindowTitle(window, newTitle.c_str());

			// Resets times and counter
			time_prev = time_crnt;
			counter = 0;

			// Use this if you have disabled VSync
			//camera.Inputs(window);
		}

		// Bind the custom framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, postproc.m_msaa_id);
		// Enable depth testing since it's disabled when drawing the framebuffer rectangle
		glEnable(GL_DEPTH_TEST);
	
		// Specify the color of the background
		glClearColor(pow(0.07f, GAMMA), pow(0.13f, GAMMA), pow(0.17f, GAMMA), 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Handles camera inputs (delete this if you have disabled VSync)
		camera.handleInput(window, stage.m_room_size, stage.m_camera_threshold);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.setReferenceMatrices(45.0f, 0.1f, 100.0f);

		// Draw the normal model
		stage.drawStage(shader_default, camera);
		
		// Make it so the multisampling FBO is read while the post-processing FBO is drawn
		glBindFramebuffer(GL_READ_FRAMEBUFFER, postproc.m_msaa_id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postproc.m_gamma_id);
		// Conclude the multisampling and copy it to the post-processing FBO
		glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		// Bind the default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Draw the framebuffer rectangle
		shader_framebuffer.useProgram();
		glBindVertexArray(postproc.m_vao_id);
		glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
		glBindTexture(GL_TEXTURE_2D, postproc.m_gamma_texture);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}

	// Delete all the objects we've created
	shader_default.deleteProgram();
	postproc.deleteFBO();
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
}