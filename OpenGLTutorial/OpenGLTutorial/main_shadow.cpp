#include"Model.h"
#include"main.h"

void mainShadow()
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
	Shader shader_default("default_shadow.vert", "default_shadow.frag");
	Shader shader_framebuffer("framebuffer.vert", "framebuffer.frag");
	Shader shader_shadowmap("shadowmap.vert", "shadowmap.frag");
	Shader shader_shadowcubemap("shadowcubemap.vert", "shadowcubemap.frag", "shadowcubemap.geom");

	// Take care of all the light related things
	glm::vec4 light_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 light_position = glm::vec3(0.0f, 10.0f, 0.0f);

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
	Camera camera(WIDTH, HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));

	// Load in models
	Model crow("models/crow/scene.gltf");
	Model ground("models/ground/scene.gltf");
	Model trees("models/trees/scene.gltf");


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

	// Create Render Buffer Object (MSAA)
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

	// Create Framebuffer Texture (Post-Processing)
	unsigned int fbtex_post_id;
	glGenTextures(1, &fbtex_post_id);
	glBindTexture(GL_TEXTURE_2D, fbtex_post_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbtex_post_id, 0);

	// Error checking framebuffer
	fbo_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fbo_status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Post-Processing Framebuffer error: " << fbo_status << std::endl;

	// Create Frame Buffer Object
	unsigned int fbo_shadowmap_id;
	glGenFramebuffers(1, &fbo_shadowmap_id);

	// Create Frame Buffer Texture (Shadow Map)
	unsigned int shadowmap_width = 2048, shadowmap_height = 2048;
	unsigned int fbtex_shadowmap_id;
	glGenTextures(1, &fbtex_shadowmap_id);
	glBindTexture(GL_TEXTURE_2D, fbtex_shadowmap_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowmap_width, shadowmap_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float clamp_color[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //So no mesh can be in shadow
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clamp_color);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_shadowmap_id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbtex_shadowmap_id, 0);
	// Needed since we don't touch the color buffer
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Matrices needed for the light's perspective
	float far_plane = 100.0f;
	// For Directional Light
	/*
	glm::mat4 orthogonal_projection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, far_plane);
	glm::mat4 light_view = glm::lookAt(20.0f * light_position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 light_projection = orthogonal_projection * light_view;
	*/

	// For Spotlight
	glm::mat4 perspective_projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, far_plane);
	glm::mat4 light_view = glm::lookAt(light_position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 light_projection = perspective_projection * light_view;
	
	// For Directional Light & Spotlight
	shader_shadowmap.useProgram();
	glUniformMatrix4fv(glGetUniformLocation(shader_shadowmap.m_id, "u_light_projection_matrix"), 1, GL_FALSE, glm::value_ptr(light_projection));

	// For Point Light
	unsigned int fbo_depthcubemap_id;
	glGenFramebuffers(1, &fbo_depthcubemap_id);

	unsigned int fbtex_depthcubemap_id;
	glGenTextures(1, &fbtex_depthcubemap_id);

	glBindTexture(GL_TEXTURE_CUBE_MAP, fbtex_depthcubemap_id);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowmap_width, shadowmap_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo_depthcubemap_id);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, fbtex_depthcubemap_id, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Matrices needed for the light's perspective on all faces of the cubemap
	glm::mat4 shadow_projection_matrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, far_plane);
	glm::mat4 shadow_transform_matrix[] =
	{
		shadow_projection_matrix * glm::lookAt(light_position, light_position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		shadow_projection_matrix * glm::lookAt(light_position, light_position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		shadow_projection_matrix * glm::lookAt(light_position, light_position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
		shadow_projection_matrix * glm::lookAt(light_position, light_position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
		shadow_projection_matrix * glm::lookAt(light_position, light_position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
		shadow_projection_matrix * glm::lookAt(light_position, light_position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))
	};

	// Export all matrices to shader
	shader_shadowcubemap.useProgram();
	glUniformMatrix4fv(glGetUniformLocation(shader_shadowcubemap.m_id, "u_shadow_matrices[0]"), 1, GL_FALSE, glm::value_ptr(shadow_transform_matrix[0]));
	glUniformMatrix4fv(glGetUniformLocation(shader_shadowcubemap.m_id, "u_shadow_matrices[1]"), 1, GL_FALSE, glm::value_ptr(shadow_transform_matrix[1]));
	glUniformMatrix4fv(glGetUniformLocation(shader_shadowcubemap.m_id, "u_shadow_matrices[2]"), 1, GL_FALSE, glm::value_ptr(shadow_transform_matrix[2]));
	glUniformMatrix4fv(glGetUniformLocation(shader_shadowcubemap.m_id, "u_shadow_matrices[3]"), 1, GL_FALSE, glm::value_ptr(shadow_transform_matrix[3]));
	glUniformMatrix4fv(glGetUniformLocation(shader_shadowcubemap.m_id, "u_shadow_matrices[4]"), 1, GL_FALSE, glm::value_ptr(shadow_transform_matrix[4]));
	glUniformMatrix4fv(glGetUniformLocation(shader_shadowcubemap.m_id, "u_shadow_matrices[5]"), 1, GL_FALSE, glm::value_ptr(shadow_transform_matrix[5]));
	glUniform3f(glGetUniformLocation(shader_shadowcubemap.m_id, "u_light_position"), light_position.x, light_position.y, light_position.z);
	glUniform1f(glGetUniformLocation(shader_shadowcubemap.m_id, "u_far_plane"), far_plane);


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
			std::string new_title = "OpenGL [" + fps + "FPS / " + ms + "ms]";
			glfwSetWindowTitle(window, new_title.c_str());

			// Resets times and counter
			time_prev = time_crnt;
			counter = 0;

			// Use this if you have disabled VSync
		}

		// Depth testing needed for Shadow Map
		glEnable(GL_DEPTH_TEST);

		// Preparations for the Shadow Map (Directional Light and Spotlight)
		glViewport(0, 0, shadowmap_width, shadowmap_height);
		
		// For Directional Light, Spotlight
		/* 
		glBindFramebuffer(GL_FRAMEBUFFER, fbo_shadowmap_id);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Draw scene for shadow map
		//crow.drawModel(shader_shadowmap, camera);
		ground.drawModel(shader_shadowmap, camera);
		trees.drawModel(shader_shadowmap, camera);

		*/
		//

		// For Point Light
		glBindFramebuffer(GL_FRAMEBUFFER, fbo_depthcubemap_id);
		glClear(GL_DEPTH_BUFFER_BIT);
		ground.drawModel(shader_shadowcubemap, camera);
		trees.drawModel(shader_shadowcubemap, camera);
		//

		// Switch back to the default framebuffer & Switch back to the default viewport
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, WIDTH, HEIGHT);

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
		camera.setReferenceMatrices(45.0f, 0.1f, far_plane);

		// Send the light matrix to the shader
		shader_default.useProgram();
		glUniformMatrix4fv(glGetUniformLocation(shader_default.m_id, "u_light_projection_matrix"), 1, GL_FALSE, glm::value_ptr(light_projection));
		// Send uniform data to Shadow Cube Map (Point Light Shadowing)
		glUniform1f(glGetUniformLocation(shader_default.m_id, "u_far_plane"), far_plane);


		// Bind the Shadow Map (Directional Light + Spotlight)
		/*
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_2D, fbtex_shadowmap_id);
		glUniform1i(glGetUniformLocation(shader_default.m_id,"u_shadowmap"), 2);
		*/

		// Bind the Shadow Map (Point Light)
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, fbtex_depthcubemap_id);
		glUniform1i(glGetUniformLocation(shader_default.m_id, "u_shadowcubemap"), 2);
		

		// Draw the normal model
		//crow.drawModel(shader_default, camera);
		ground.drawModel(shader_default, camera);
		trees.drawModel(shader_default, camera);
		
		// Make it so the multisampling FBO is read while the post-processing FBO is drawn
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo_msaa_id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo_post_id);
		// Conclude the multisampling and copy it to the post-processing FBO
		glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);


		// Bind the default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// Draw the framebuffer rectangle
		shader_framebuffer.useProgram();
		glBindVertexArray(vao_rect);
		glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, fbtex_post_id);
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