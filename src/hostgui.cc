#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "hostgui.h"

HostGui::HostGui(){
    thread_ = std::thread{&HostGui::ThreadMain, this};
}
HostGui::~HostGui(){
    if(thread_.joinable()){
        thread_.join();
    }
}

void HostGui::ThreadMain(){
    glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()){
        return;
    }

	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH_, SCR_HEIGHT_, "Imgui Test", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, HostGui::framebuffer_size_callback);
	glfwSwapInterval(1); // Enable vsync

	bool err = gl3wInit() != 0;
	if (err){
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return;
	}

	//==========================================================================

	glEnable(GL_DEPTH_TEST);

	Shader shader{"./src/vertex_shader.vs", "./src/fragment_shader.fs"};
	Shader shader_strip_line{"./src/line_strip.vs", "./src/line_strip.fs"};

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// load and create a texture
    // -------------------------
    unsigned int texture1;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load("./resources/container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

	// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    shader.use();
    shader.setInt("texture1", 0);

	// ============================================================

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	io.Fonts->AddFontFromFileTTF("./resources/DroidSans.ttf", 20.0f);

	constexpr unsigned int size = 50;
	float omega = 3.141592654f*2.f/static_cast<float>(size);
	std::vector<float> path;
	float r = 2.f;
	for(auto i=0; i<size; i++){
		path.push_back(cos(static_cast<float>(i)*omega) * r);
		path.push_back(0.f);
		path.push_back(sin(static_cast<float>(i)*omega) * r);
	}

	while (!glfwWindowShouldClose(window) && is_on_){
		// input
		processInput(window);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		GuiWindow();

		// Rendering
		ImGui::Render();
		// glfwMakeContextCurrent(window);
		glClearColor(0.9f, 0.9f, 0.9f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

		shader.use();

		// create transformations
        glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view;//          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);
        model = glm::rotate(model, /*(float)glfwGetTime()*/30.f, glm::vec3(0.f, 1.0f, 0.0f));
		// float radius = 5.f;
		// float camX = sin(glfwGetTime()) * radius;
		// float camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(0, 4.0f, -3.f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH_ / (float)SCR_HEIGHT_, 0.1f, 100.0f);
        // retrieve the matrix uniform locations
        unsigned int modelLoc = glGetUniformLocation(shader.ID, "model");
        unsigned int viewLoc  = glGetUniformLocation(shader.ID, "view");
        // pass them to the shaders (3 different ways)
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        shader.setMat4("projection", projection);

		float v0[] = {
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			0.5f, -0.5f, -0.5f,  2.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  2.0f, 2.0f,
			0.5f,  0.5f, -0.5f,  2.0f, 2.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 2.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f
		};
		float v1[] = {
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f
		};
		std::vector<float*> v;
		v.push_back(v0);
		v.push_back(v1);

		for(auto i=0; i<v.size(); i++){
			unsigned int VAO, VBO;
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(v0), v[i], GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    		glEnableVertexAttribArray(1);
			glBindVertexArray(VAO);
        	glDrawArrays(GL_TRIANGLES, 0, sizeof(v0)/sizeof(float));
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
		}

		RenderPoints(path, shader_strip_line, model, view, projection);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

    is_on_ = false;
}

void HostGui::RenderPoints(std::vector<float> data, Shader &shader, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection){
	shader.use();
	shader.setMat4("model", model);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);
	auto size = data.size();

	unsigned int vao_stripline, vbo_stripline;
	glGenVertexArrays(1, &vao_stripline);
	glGenBuffers(1, &vbo_stripline);
	glBindVertexArray(vao_stripline);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_stripline);
	glBufferData(GL_ARRAY_BUFFER, size*sizeof(float), data.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glPointSize(2);

	glBindVertexArray(vao_stripline);
	glDrawArrays(GL_POINTS, 0, size/3);
	glDeleteVertexArrays(1, &vao_stripline);
	glDeleteBuffers(1, &vbo_stripline);
}

void HostGui::GuiWindow(){
	ImGui::Begin("Hello, world!");

	static bool show_demo_window = false;
	ImGui::Checkbox("Demo Window", &show_demo_window);

	static float f = 0.0f;
	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();

	if(show_demo_window){
		ImGui::ShowDemoWindow(&show_demo_window);
	}
}

void HostGui::framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);
}
void HostGui::processInput(GLFWwindow *window){
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
void HostGui::glfw_error_callback(int error, const char* description){
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}