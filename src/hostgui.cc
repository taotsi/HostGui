#define STB_IMAGE_IMPLEMENTATION
// dont include this file anywhere else.
// dont include it in a header file;
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

void HostGui::TurnOff(){
	is_on_ = false;
}

void HostGui::TurnOn(){
	is_on_ = true;
}

void HostGui::ThreadMain(){
    glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()){
		std::cout << "Failed to init glfw\n";
        return;
    }

	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH_, SCR_HEIGHT_, "Host", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		std::cout << "Failed to create glfw window\n";
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, HostGui::framebuffer_size_callback);
	glfwSwapInterval(1); // Enable vsync

	bool err = gl3wInit() != 0;
	if (err){
		std::cout << "Failed to initialize OpenGL loader!\n";
		return;
	}

	glEnable(GL_DEPTH_TEST);

	/* Shaders here */
	Shader shader{"./src/vertex_shader.vs", "./src/fragment_shader.fs"};
	Shader shader_strip_line{"./src/line_strip.vs", "./src/line_strip.fs"};
	Shader shader_test{"./src/test_shader.vs", "./src/test_shader.fs"};


	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// TODO: class Texture
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load("./resources/container.jpg", &width, &height, &nrChannels, 0);
    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }else{
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    shader.use();
    shader.setInt("texture1", 0);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	io.Fonts->AddFontFromFileTTF("./resources/DroidSans.ttf", 20.0f);

	bool show_demo_window = false;

	while (!glfwWindowShouldClose(window) && is_on_){
		processInput(window);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		/* Render ImGui here */
		MainPanel();

		ImGui::Render();
		// glfwMakeContextCurrent(window);
		glClearColor(0.9f, 0.9f, 0.9f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

		// TODO: package this section about transformation
        glm::mat4 model         = glm::mat4(1.0f);
        glm::mat4 view;//          = glm::mat4(1.0f);
        glm::mat4 projection    = glm::mat4(1.0f);
        model = glm::rotate(model, /*(float)glfwGetTime()*/30.f, glm::vec3(0.f, 1.0f, 0.0f));
		// float radius = 5.f;
		// float camX = sin(glfwGetTime()) * radius;
		// float camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(0, 4.0f, -3.f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH_ / (float)SCR_HEIGHT_, 0.1f, 100.0f);

		/* Render OpenGL primitives here */
		RenderTest(std::vector<float>{}, shader_test, model, view, projection);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glViewport(0, 0, 500, 600);

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



void HostGui::MainPanel(){
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

void HostGui::RenderTest(std::vector<float> data, Shader &shader, glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection){
	shader.use();
	shader.setMat4("model", model);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left
         0.5f, -0.5f, 0.0f, // right
         0.0f,  0.5f, 0.0f  // top
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
}

void HostGui::framebuffer_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, 0.6*width, 0.6*height);
}
void HostGui::processInput(GLFWwindow *window){
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
void HostGui::glfw_error_callback(int error, const char* description){
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}