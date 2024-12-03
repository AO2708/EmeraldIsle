#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <render/shader.h>

#include <iostream>
#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <../stb/stb_image_write.h>

#include <features/Tree.h>
#include <features/Island.h>
#include <features/Pannel.h>

static GLFWwindow *window;
static int windowWidth = 1024;
static int windowHeight = 768;
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// OpenGL camera view parameters
static glm::vec3 eye_center(0.0f, 30.0f, 30.0f);
static glm::vec3 lookat(0, 0, 0);
static glm::vec3 up(0, 1, 0);
static float FoV = 50.0f;
static float zNear = 1.0f;
static float zFar = 500.0f;

// View control
static float viewAzimuth = -glm::pi<float>()/2;
static float viewPolar = -glm::pi<float>() / 4;
static float viewDistance = glm::length(eye_center - lookat);

// Lighting control
const glm::vec3 wave500(0.0f, 255.0f, 146.0f);
const glm::vec3 wave600(255.0f, 190.0f, 0.0f);
const glm::vec3 wave700(205.0f, 0.0f, 0.0f);
const glm::vec3 lightIntensity = 10000.0f * glm::vec3(255.0f, 244.0f, 214.0f);
const glm::vec3 lightPosition(-75.0f, 100.0f, -75.0f);

// Shadow mapping
static glm::vec3 lightUp = glm::normalize(glm::vec3(0.333, 0.333, 0.883));
static glm::vec3 lightLookAt(0.0f, 0.0f, 0.0f);
static int shadowMapWidth = 1024;
static int shadowMapHeight = 1024;
static float depthFoV = 100.0f;
static float depthNear = 100.0f;
static float depthFar = 500.0f;
GLuint fbo;
GLuint depthTexture;
static bool saveDepth = true;

static void saveDepthTexture(GLuint fbo, std::string filename) {
	int width = shadowMapWidth;
	int height = shadowMapHeight;
	if (shadowMapWidth == 0 || shadowMapHeight == 0) {
		width = windowWidth;
		height = windowHeight;
	}
	int channels = 3;

	std::vector<float> depth(width * height);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glReadBuffer(GL_DEPTH_COMPONENT);
	glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depth.data());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::vector<unsigned char> img(width * height * 3);
	for (int i = 0; i < width * height; ++i) img[3*i] = img[3*i+1] = img[3*i+2] = depth[i] * 255;
	stbi_write_png(filename.c_str(), width, height, channels, img.data(), width * channels);
}


struct AxisXYZ {
    // A structure for visualizing the global 3D coordinate system

	GLfloat vertex_buffer_data[18] = {
		// X axis
		0.0, 0.0f, 0.0f,
		100.0f, 0.0f, 0.0f,

		// Y axis
		0.0f, 0.0f, 0.0f,
		0.0f, 100.0f, 0.0f,

		// Z axis
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 100.0f,
	};

	GLfloat color_buffer_data[18] = {
		// X, red
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		// Y, green
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		// Z, blue
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
	};

	// OpenGL buffers
	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint colorBufferID;

	// Shader variable IDs
	GLuint mvpMatrixID;
	GLuint programID;

	void initialize() {
		// Create a vertex array object
		glGenVertexArrays(1, &vertexArrayID);
		glBindVertexArray(vertexArrayID);

		// Create a vertex buffer object to store the vertex data
		glGenBuffers(1, &vertexBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

		// Create a vertex buffer object to store the color data
		glGenBuffers(1, &colorBufferID);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

		// Create and compile our GLSL program from the shaders
		programID = LoadShadersFromFile("../EmeraldIsle/shader/axis.vert", "../EmeraldIsle/shader/axis.frag");
		if (programID == 0)
		{
			std::cerr << "Failed to load shaders." << std::endl;
		}

		// Get a handle for our "MVP" uniform
		mvpMatrixID = glGetUniformLocation(programID, "MVP");
	}

	void render(glm::mat4 cameraMatrix) {
		glUseProgram(programID);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glm::mat4 mvp = cameraMatrix;
		glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

        // Draw the lines
        glDrawArrays(GL_LINES, 0, 6);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}

	void cleanup() {
		glDeleteBuffers(1, &vertexBufferID);
		glDeleteBuffers(1, &colorBufferID);
		glDeleteVertexArrays(1, &vertexArrayID);
		glDeleteProgram(programID);
	}
};


int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW." << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Lab 1", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to open a GLFW window." << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
	int version = gladLoadGL(glfwGetProcAddress);
	if (version == 0)
	{
		std::cerr << "Failed to initialize OpenGL context." << std::endl;
		return -1;
	}

	// Prepare shadow map size for shadow mapping. Usually this is the size of the window itself, but on some platforms like Mac this can be 2x the size of the window. Use glfwGetFramebufferSize to get the shadow map size properly.
	glfwGetFramebufferSize(window, &shadowMapWidth, &shadowMapHeight);

	// Background
	glClearColor(0.2f, 0.2f, 0.2f, 0.f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Create and bind the FBO
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	// Create the 2D texture that we will use as the framebuffer's depth buffer
	glGenTextures(1,&depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Attach depth texture to framebuffer's depth buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Initialisation
	Tree tree;
	tree.initialize(glm::vec3(0.0,0.0,0.0), glm::vec3(1.0,1.0,1.0));

	AxisXYZ axis;
	axis.initialize();

	Pannel pannel;
	pannel.initialize(glm::vec3(0.0,0.0,0.0), glm::vec3(1.0,1.0,1.0));

	Island island;
	island.initialize(glm::vec3(0.0,0.0,0.0), glm::vec3(2.0,2.0,2.0));

	// Camera setup for light
	glm::mat4 viewMatrixLight, projectionMatrixLight;
	projectionMatrixLight = glm::perspective(glm::radians(depthFoV), (float)shadowMapWidth / shadowMapHeight, depthNear, depthFar);

	// Camera setup
	glm::mat4 viewMatrix, projectionMatrix;
	projectionMatrix = glm::perspective(glm::radians(FoV), (float)windowWidth / windowHeight, zNear, zFar);

	do
	{
		// Render the depth map
		glViewport(0,0,shadowMapWidth,shadowMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_DEPTH_BUFFER_BIT);

		viewMatrixLight = glm::lookAt(lightPosition, lightLookAt, lightUp);
		glm::mat4 vpLight = projectionMatrixLight * viewMatrixLight;

		tree.renderShadow(vpLight);
		island.renderShadow(vpLight);
		pannel.renderShadow(vpLight);
		if (saveDepth) {
			std::string filename = "../EmeraldIsle/depth_camera.png";
			saveDepthTexture(fbo, filename);
			std::cout << "Depth texture saved to " << filename << std::endl;
			saveDepth = false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER,0);

		// Render the scene
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		viewMatrix = glm::lookAt(eye_center, lookat, up);
		glm::mat4 vp = projectionMatrix * viewMatrix;

		tree.render(vp, vpLight);
		axis.render(vp);
		pannel.render(vp,vpLight);
		island.render(vp,vpLight);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (!glfwWindowShouldClose(window));

	// Clean up
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &depthTexture);
	tree.cleanup();
	axis.cleanup();
	island.cleanup();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	float moveSpeed = 1.0f;
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		eye_center = glm::vec3(0.0f, 30.0f, 30.0f);
		lookat = glm::vec3(0, 0, 0);
		viewAzimuth = -glm::pi<float>()/2;
		viewPolar = -glm::pi<float>() / 4;
		std::cout << "Reset." << std::endl;
	}

	// Moving
	if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		glm::vec3 direction = normalize(lookat - eye_center);
		eye_center -= moveSpeed * direction;
		lookat -= moveSpeed * direction;
	}
	if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		glm::vec3 direction = normalize(lookat - eye_center);
		eye_center += moveSpeed * direction;
		lookat += moveSpeed * direction;
	}
	if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		glm::vec3 direction = normalize(lookat - eye_center);
		glm::vec3 right = normalize(cross(direction, up));
		eye_center -= moveSpeed * right;
		lookat -= moveSpeed * right;
	}
	if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS)) {
		glm::vec3 direction = normalize(lookat - eye_center);
		glm::vec3 right = normalize(cross(direction, up));
		eye_center += moveSpeed * right;
		lookat += moveSpeed * right;
	}

	// Rotation
	if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		viewPolar += 0.1f;
		lookat.y = eye_center.y + viewDistance * sin(viewPolar);
		lookat.x = eye_center.x + viewDistance * cos(viewPolar) * cos(viewAzimuth);
		lookat.z = eye_center.z + viewDistance * cos(viewPolar) * sin(viewAzimuth);
	}

	if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		viewPolar -= 0.1f;
		lookat.y = eye_center.y + viewDistance * sin(viewPolar);
		lookat.x = eye_center.x + viewDistance * cos(viewPolar) * cos(viewAzimuth);
		lookat.z = eye_center.z + viewDistance * cos(viewPolar) * sin(viewAzimuth);
	}

	if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		viewAzimuth -= 0.1f;
		lookat.y = eye_center.y + viewDistance * sin(viewPolar);
		lookat.x = eye_center.x + viewDistance * cos(viewPolar) * cos(viewAzimuth);
		lookat.z = eye_center.z + viewDistance * cos(viewPolar) * sin(viewAzimuth);
	}

	if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		viewAzimuth += 0.1f;
		lookat.y = eye_center.y + viewDistance * sin(viewPolar);
		lookat.x = eye_center.x + viewDistance * cos(viewPolar) * cos(viewAzimuth);
		lookat.z = eye_center.z + viewDistance * cos(viewPolar) * sin(viewAzimuth);
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (yoffset > 0) {
		eye_center.y += 1.0;
		lookat.y += 1.0;
	}
	if (yoffset < 0) {
		eye_center.y -= 1.0;
		lookat.y -= 1.0;
	}
}
