#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <render/shader.h>

#include <iostream>
#include <iomanip>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <feature/Forest.h>
#include <feature/Island.h>
#include <feature/Pannel.h>
#include <feature/Robot.h>
#include <feature/Sea.h>

static GLFWwindow *window;
static int windowWidth = 1024;
static int windowHeight = 768;
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// OpenGL camera view parameters
glm::vec3 eye_center(20.0,1.5,-55.0);
static glm::vec3 lookat(0, 15, 0);
static glm::vec3 up(0, 1, 0);
static float FoV = 50.0f;
static float zNear = 1.0f;
static float zFar = 500.0f;

// View control
static float viewAzimuth = 1.92;
static float viewPolar = 0.23;
static float viewDistance = glm::length(eye_center - lookat);

// Lighting control
const glm::vec3 wave500(0.0f, 255.0f, 146.0f);
const glm::vec3 wave600(255.0f, 190.0f, 0.0f);
const glm::vec3 wave700(205.0f, 0.0f, 0.0f);
const glm::vec3 lightIntensity = 10000.0f * glm::vec3(255.0f, 244.0f, 214.0f);
const glm::vec3 lightPosition(-50.0f, 100.0f, -50.0f);

// Shadow mapping
static glm::vec3 lightUp = glm::normalize(glm::vec3(0.333, 0.333, 0.883));
static glm::vec3 lightLookAt(0.0f, 0.0f, 0.0f);
static int shadowMapWidth = 1024;
static int shadowMapHeight = 1024;
static float depthFoV = 65.0f;
static float depthNear = 70.0f;
static float depthFar = 500.0f;
GLuint fbo;
GLuint depthTexture;
static bool saveDepth = false;

// Animation
static bool playAnimation = true;
static float playbackSpeed = 1.5f;

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
	window = glfwCreateWindow(1024, 768, "Emerald Isle", NULL, NULL);
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
	Forest forestEastNorth;
	forestEastNorth.initialize(glm::vec3(-28.0,0.0,32.0), glm::vec3(0.5,0.5,0.5), 15);

	Forest forestWestNorth;
	forestWestNorth.initialize(glm::vec3(28.0,0.0,32.0), glm::vec3(0.5,0.5,0.5), 10);

	Pannel pannel;
	pannel.initialize(glm::vec3(20.0,-0.2,-48.0), glm::vec3(1.0,1.0,1.0), glm::radians(235.0f));

	Island island;
	island.initialize(glm::vec3(0.0,0.0,0.0), glm::vec3(5.0,5.0,5.0));

	Sea sea;
	glm::vec3 seaPosition = eye_center;
	seaPosition.y = 0.0f;
	sea.initialize(seaPosition, glm::vec3(20.0,20.0,20.0));

	Robot robot;
	robot.initialize(glm::vec3(-14.0,0.0,-46.0), glm::vec3(0.5,0.5,0.5), glm::radians(90.0f));

	// Camera setup for light
	glm::mat4 viewMatrixLight, projectionMatrixLight;
	projectionMatrixLight = glm::perspective(glm::radians(depthFoV), (float)shadowMapWidth / shadowMapHeight, depthNear, depthFar);

	// Camera setup
	glm::mat4 viewMatrix, projectionMatrix;
	projectionMatrix = glm::perspective(glm::radians(FoV), (float)windowWidth / windowHeight, zNear, zFar);

	// Time and frame rate tracking
	static double lastTime = glfwGetTime();
	float time = 0.0f;
	float fTime = 0.0f;
	unsigned long frames = 0;
	do
	{
		// Render the depth map
		glViewport(0,0,shadowMapWidth,shadowMapHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_DEPTH_BUFFER_BIT);

		viewMatrixLight = glm::lookAt(lightPosition, lightLookAt, lightUp);
		glm::mat4 vpLight = projectionMatrixLight * viewMatrixLight;

		forestEastNorth.renderShadow(vpLight);
		forestWestNorth.renderShadow(vpLight);
		island.renderShadow(vpLight);
		pannel.renderShadow(vpLight);
		robot.renderShadow(vpLight);
		if (saveDepth) {
			FileHandler fileSaver;
			std::string filename = "../EmeraldIsle/depth_camera.png";
			fileSaver.saveDepthTexture(fbo, filename, shadowMapWidth, shadowMapHeight);
			std::cout << "Depth texture saved to " << filename << std::endl;
			saveDepth = false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER,0);

		// Render the scene
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update states for animation
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);
		lastTime = currentTime;

		if (playAnimation) {
			time += deltaTime * playbackSpeed;
			robot.update(time);
		}

		viewMatrix = glm::lookAt(eye_center, lookat, up);
		glm::mat4 vp = projectionMatrix * viewMatrix;

		forestEastNorth.render(vp, vpLight);
		forestWestNorth.render(vp, vpLight);
		pannel.render(vp,vpLight);
		island.render(vp,vpLight);
		sea.render(vp,vpLight);
		robot.render(vp,vpLight);

		// FPS tracking
		// Count number of frames over a few seconds and take average
		frames++;
		fTime += deltaTime;
		if (fTime > 2.0f) {
			float fps = frames / fTime;
			frames = 0;
			fTime = 0;

			std::stringstream stream;
			stream << std::fixed << std::setprecision(2) << "Emerald Isle | Frames per second (FPS): " << fps;
			glfwSetWindowTitle(window, stream.str().c_str());
		}

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (!glfwWindowShouldClose(window));

	// Clean up
	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &depthTexture);
	forestEastNorth.cleanup();
	forestWestNorth.cleanup();
	island.cleanup();
	pannel.cleanup();
	sea.cleanup();
	robot.cleanup();

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
		eye_center = glm::vec3(20.0,1.5,-55.0);
		lookat = glm::vec3(0, 15, 0);
		viewAzimuth = 1.92;
		viewPolar = 0.23;
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
