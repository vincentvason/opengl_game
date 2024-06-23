#include"Model.h"
#include"main.h"

void mainExtendMap()
{
	const unsigned int WIDTH = 800;
	const unsigned int HEIGHT = 800;

	// Number of MSAA_SAMPLES per pixel for MSAA
	unsigned int MSAA_SAMPLES = 8;

	// Controls the gamma function
	float gamma = 2.2f;


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

	// Vertices for plane with texture
	// First Tutorial (does not need TBN)
	
	std::vector<Vertex> vertices =
	{
		Vertex{glm::vec3(-1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
		Vertex{glm::vec3(-1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
		Vertex{glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
		Vertex{glm::vec3(1.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)}
	};
	

	// Indices for plane with texture
	std::vector<GLuint> indices =
	{
		0, 1, 2,
		0, 2, 3
	};

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
	// No Bloom
	//Shader shader_default("default_extendmap.vert", "default_extendmap.frag", "default_extendmap.geom");
	// Bloom
	Shader shader_default("default_extendmap.vert", "default_extendmap2.frag", "default_extendmap.geom");
	Shader shader_framebuffer("framebuffer.vert", "framebuffer.frag");
	Shader shader_blur("framebuffer.vert", "blur.frag");

	// Take care of all the light related things
	glm::vec4 light_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 light_position = glm::vec3(0.5f, 0.5f, 0.5f);

	shader_default.useProgram();
	glUniform4f(glGetUniformLocation(shader_default.m_id, "u_light_color"), light_color.x, light_color.y, light_color.z, light_color.w);
	glUniform3f(glGetUniformLocation(shader_default.m_id, "u_light_position"), light_position.x, light_position.y, light_position.z);
	shader_framebuffer.useProgram();
	glUniform1i(glGetUniformLocation(shader_framebuffer.m_id, "u_screen_texture"), 0);
	glUniform1f(glGetUniformLocation(shader_framebuffer.m_id, "u_gamma"), gamma);

	// Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);

	// Enables Multisampling
	glEnable(GL_MULTISAMPLE);

	// Enables Cull Facing
	glEnable(GL_CULL_FACE);
	// Keeps front faces
	glCullFace(GL_FRONT);

	// Uses counter clock-wise standard
	glFrontFace(GL_CCW);


	// Creates camera object
	Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f), 0.005f);

	// Prepare framebuffer rectangle VBO and VAO
	unsigned int vao_rect, vbo_rect;
	glGenVertexArrays(1, &vao_rect);
	glGenBuffers(1, &vbo_rect);
	glBindVertexArray(vao_rect);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_rect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle_vertices), &rectangle_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));



	// Variables to create periodic event for FPS displaying
	double time_prev = 0.0;
	double time_crnt = 0.0;
	double time_diff;
	// Keeps track of the amount of frames in time_diff
	unsigned int counter = 0;

	// Use this to disable VSync (not advized)
	//glfwSwapInterval(0);


	// Create Frame Buffer Object
	unsigned int fbo_msaa_id;
	glGenFramebuffers(1, &fbo_msaa_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_msaa_id);

	// Create Framebuffer Texture
	unsigned int fbtex_msaa_id;
	glGenTextures(1, &fbtex_msaa_id);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fbtex_msaa_id);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGB16F, WIDTH, HEIGHT, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	glTexParameteri(GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Prevents edge bleeding
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, fbtex_msaa_id, 0);

	// Create Render Buffer Object
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);


	// Error checking framebuffer
	auto fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fbo_status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer error: " << fbo_status << std::endl;

	// Create Frame Buffer Object
	unsigned int fbo_post_id;
	glGenFramebuffers(1, &fbo_post_id);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_post_id);

	// Create Framebuffer Texture
	unsigned int fbtex_post_id;
	glGenTextures(1, &fbtex_post_id);
	glBindTexture(GL_TEXTURE_2D, fbtex_post_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbtex_post_id, 0);

	//Blooming Feature
	unsigned int fbtex_bloom_id;
	glGenTextures(1, &fbtex_bloom_id);
	glBindTexture(GL_TEXTURE_2D, fbtex_bloom_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, fbtex_bloom_id, 0);
	
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	// Error checking framebuffer
	fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fbo_status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Post-Processing Framebuffer error: " << fbo_status << std::endl;

	unsigned int fbo_pingpong_id[2];
	unsigned int fbtex_pingpong_id[2];
	glGenTextures(2, fbo_pingpong_id);

	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, fbtex_pingpong_id[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, fbtex_bloom_id, 0);

		fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (fbo_status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Ping-Pong on " << std::to_string(i) << "Framebuffer error : " << fbo_status << std::endl;
		}
	}
	
	// Load things
	std::vector<Texture> textures =
	{
		Texture("textures2/diffuse.png", Texture::DIFFUSE, 0)
	};

	// Plane with the texture
	Mesh plane(vertices, indices, textures);
	// Normal map for the plane
	Texture normal_map_texture("textures2/normal.png", Texture::NORMAL, 1);
	Texture displacement_map_texture("textures2/displacement.png", Texture::DISPLACEMENT, 2, GL_LINEAR);


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
		glBindFramebuffer(GL_FRAMEBUFFER, fbo_msaa_id);
		// Specify the color of the background
		glClearColor(pow(0.07f, gamma), pow(0.13f, gamma), pow(0.17f, gamma), 1.0f);
		// Clean the back buffer and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Enable depth testing since it's disabled when drawing the framebuffer rectangle
		glEnable(GL_DEPTH_TEST);

		// Handles camera inputs (delete this if you have disabled VSync)
		camera.handleInput(window);
		// Updates and exports the camera matrix to the Vertex Shader
		camera.setReferenceMatrices(45.0f, 0.1f, 100.0f);

		shader_default.useProgram();
		normal_map_texture.bindTexture();
		glUniform1i(glGetUniformLocation(shader_default.m_id, "u_normal0"), 1);
		displacement_map_texture.bindTexture();
		glUniform1i(glGetUniformLocation(shader_default.m_id, "u_displacement0"), 2);

		// Draw the normal model
		plane.drawMesh(shader_default, camera);

		// Make it so the multisampling FBO is read while the post-processing FBO is drawn
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_msaa_id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_post_id);
		// Conclude the multisampling and copy it to the post-processing FBO
		glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

		// Blur Effect
		bool horizontal = true, first_iter = true;
		int amount = 8;
		shader_blur.useProgram();
		for (unsigned int i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, fbo_pingpong_id[horizontal]);
			glUniform1i(glGetUniformLocation(shader_blur.m_id, "u_horizontal"), horizontal);

			// In the first bounc we want to get the data from the bloomTexture
			if (first_iter)
			{
				glBindTexture(GL_TEXTURE_2D, fbtex_bloom_id);
				first_iter = false;
			}
			// Move the data between the pingPong textures
			else
			{
				glBindTexture(GL_TEXTURE_2D, fbtex_pingpong_id[!horizontal]);
			}

			// Render the image
			glBindVertexArray(vao_rect);
			glDisable(GL_DEPTH_TEST);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			// Switch between vertical and horizontal blurring
			horizontal = !horizontal;
		}


		// Bind the default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Draw the framebuffer rectangle
		shader_framebuffer.useProgram();
		glBindVertexArray(vao_rect);
		glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
		//glBindTexture(GL_TEXTURE_2D, fbtex_post_id);
		//glBindTexture(GL_TEXTURE_2D, fbtex_bloom_id);
		glBindTexture(GL_TEXTURE_2D, fbtex_pingpong_id[!horizontal]);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		// Swap the back buffer with the front buffer
		glfwSwapBuffers(window);
		// Take care of all GLFW events
		glfwPollEvents();
	}



	// Delete all the objects we've created
	shader_default.deleteProgram();
	glDeleteFramebuffers(1, &fbo_msaa_id);
	glDeleteFramebuffers(1, &fbo_post_id);
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
}