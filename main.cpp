#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"

static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void update(GLFWwindow* window);
static void move(GLFWwindow* window);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

const int SCR_WIDTH = 800, SCR_HEIGHT = 600;

Camera camera({ 0.0f, 0.0f, -10.0f });

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

	glm::mat4 projection, model;
	model = glm::mat4();
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);

	shader.use();
	shader.setMat4("model", model);
	shader.setMat4("proj", projection);
	shader.setMat4("view", camera.getViewNatrix());
	shader.setVec3("viewPos", camera.getPosition());

	Model statue("Resources/statue/statue.obj");
	Model sphere("Resources/sphere2.obj");
	//Model cube("Resources/cube.obj");


	while (!glfwWindowShouldClose(window)) {
		update(window);

		shader.setMat4("view", camera.getViewNatrix());
		shader.setVec3("viewPos", camera.getPosition());

		model = glm::mat4();
		shader.setMat4("model", model);
		statue.render(shader);

		model = translate(model, {10.0 ,0.0 ,0.0});
		shader.setMat4("model", model);
		sphere.render(shader);

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

		camera.rotate({ dx * speed, 0.0f, 0.0f });
		camera.rotate({ 0.0f, dy * speed, 0.0f });
	}
}