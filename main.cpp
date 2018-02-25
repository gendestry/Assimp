#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Shader.h"
#include "Camera.h"
#include "Object.h"
#include "Cubemap.h"

static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void update(GLFWwindow* window);
static void move(GLFWwindow* window);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

const int SCR_WIDTH = 800, SCR_HEIGHT = 600;

Camera camera({ 0.0f, 0.0f, 10.0f });

int main() {
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Assimp improving", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glewInit() != 0)
		return -1;

	/* ------------------- ACTUAL CODE ------------------- */

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.4, 0.8, 1.0);

	Shader shader("modelvert.glsl", "modelfrag.glsl");
	Shader skyboxShader("skyboxvert.glsl", "skyboxfrag.glsl");
	Shader refshader("refvert.glsl", "reffrag.glsl");

	Object tree("Resources/polybridge_tree.obj", { 0.0, -2.0, -10.0 });
	Object sphere("Resources/sphere2.obj", { 2.0, -1.0, 2.0 });
	Object cube("Resources/cube.obj");
	Object plane("Resources/plane.obj", { 0.0, -5.0, 0.0 });
	plane.scale({2.0, 1.0, 2.0 });

	Cubemap skybox({ "Resources/skybox/right.jpg","Resources/skybox/left.jpg","Resources/skybox/top.jpg","Resources/skybox/bottom.jpg","Resources/skybox/front.jpg","Resources/skybox/back.jpg" });

	glm::mat4 projection, model;
	model = glm::mat4();
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);

	shader.use();
	shader.setMat4("model", model);
	shader.setMat4("proj", projection);
	shader.setMat4("view", camera.getViewNatrix());
	shader.setVec3("viewPos", camera.getPosition());

	skyboxShader.use();
	skyboxShader.setMat4("proj", projection);
	skyboxShader.setMat4("view", camera.getViewNatrix());
	skyboxShader.setInt("skybox", 0);

	refshader.use();
	refshader.setMat4("proj", projection);
	refshader.setMat4("view", camera.getViewNatrix());
	refshader.setVec3("viewPos", camera.getPosition());
	refshader.setInt("skybox", 0);

	while (!glfwWindowShouldClose(window)) {
		update(window);

		shader.use();
		shader.setMat4("view", camera.getViewNatrix());
		shader.setVec3("viewPos", camera.getPosition());

		tree.rotate({ 0.0, 0.01, 0.0 });
		tree.render(shader);
		sphere.render(shader);
		plane.render(shader);

		refshader.use();
		refshader.setMat4("view", camera.getViewNatrix());
		refshader.setVec3("viewPos", camera.getPosition());
		cube.render(refshader);

		skyboxShader.use();
		skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.getViewNatrix())));
		glDepthFunc(GL_LEQUAL);
		skybox.bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

static void update(GLFWwindow* window) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	move(window);
}

static void move(GLFWwindow* window) {
	float speed = 0.06f; // temp

	if (glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	/* LEFT -RIGHT */
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.move(LEFT, speed);
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.move(RIGHT, speed);

	/* FORWARD - BACKWARD */
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.move(FORWARD, speed);
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.move(BACKWARD, speed);

	/* UP - DOWN */
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.move(UP, speed);
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.move(DOWN, speed);
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	static double oldX, oldY;
	static bool first = true;
	float speed = 0.07f;

	if (first) {
		oldX = xpos;
		oldY = ypos;
		first = false;
	}
	else {
		double dx = xpos - oldX;
		double dy = ypos - oldY;

		oldX = xpos;
		oldY = ypos;

		camera.rotate({ dy * speed, 0.0f, 0.0f });
		camera.rotate({ 0.0f, dx * speed, 0.0f });
	}
}