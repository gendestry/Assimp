#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Camera.h"

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
static void move();

float deltaTime = 0.0f;
float lastFrame = 0.0f;

const int SCR_WIDTH = 800, SCR_HEIGHT = 600;

GLFWwindow* window;
Shader* shader;
Camera* camera;

int main() {
	
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hello World", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	if (glewInit() != 0)
		return -1;

	/* ACTUAL CODE */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	shader = new Shader("vert.glsl", "frag.glsl");
	camera = new Camera({ 0.0f, 0.0f, -10.0f });

	float vertices[] = {
		 // front
		 1.0f,  1.0f, 1.0f,		
		-1.0f,  1.0f, 1.0f,	    
		-1.0f, -1.0f, 1.0f,	    
		-1.0f, -1.0f, 1.0f,	    
		 1.0f, -1.0f, 1.0f,	    
		 1.0f,  1.0f, 1.0f,

		 // back
		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		 // left
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,

		 // right
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,

		 // top
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,

		 // bottom
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
	    -1.0f, -1.0f,  1.0f,
	    -1.0f, -1.0f, -1.0f,
	};

	float normals[] = {
		// front
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		// back
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		// left
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,

		// right
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		// top
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		// bottom
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f
	};

	GLuint vao, vbo, nbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &nbo);
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glm::mat4 projection, view, model;
	model = glm::mat4(1.0);
	view = camera->getViewNatrix();
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	shader->use();

	shader->setMat4("model", model);
	shader->setMat4("view", view);
	shader->setMat4("proj", projection);

	glClearColor(0.0, 0.4, 0.8,1.0);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		move();

		model = glm::mat4(1.0);
		shader->setMat4("model", model);
		shader->setVec3("objectColor", { 0.8, 0.3, 0.0 });
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		model = glm::translate(model, { 5.0, 0.0, 5.0 });
		shader->setMat4("model", model);
		shader->setVec3("objectColor", { 0.2, 0.3, 0.8 });
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		model = glm::translate(model, { -13.0, 2.0, 3.0 });
		shader->setMat4("model", model);
		shader->setVec3("objectColor", { 0.0, 0.8, 0.3 });
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		model = glm::mat4(1.0);
		model = glm::translate(model, { -3.0, -2.0, 4.0 });
		shader->setMat4("model", model);
		shader->setVec3("objectColor", { 0.8, 0.2, 0.8 });
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &nbo);
	glDeleteVertexArrays(1, &vao);

	delete camera;
	delete shader;

	glfwTerminate();
	return 0;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	static double oldX, oldY;
	static bool first = true;
	float speed = 0.1f;

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

		camera->rotate({ dx * speed, 0.0f, 0.0f });
		camera->rotate({ 0.0f, dy * speed, 0.0f });
		shader->setMat4("view", camera->getViewNatrix());
	}
}

void move() {
	float speed = 0.08f; // temp

	/* LEFT -RIGHT */
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera->move({ speed, 0, 0 });
		shader->setMat4("view", camera->getViewNatrix());
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera->move({ -speed, 0, 0 });
		shader->setMat4("view", camera->getViewNatrix());
	}

	/* FORWARD - BACKWARD */
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera->move({ 0, 0, speed });
		shader->setMat4("view", camera->getViewNatrix());
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera->move({ 0, 0, -speed });
		shader->setMat4("view", camera->getViewNatrix());
	}

	/* UP - DOWN */
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera->move({ 0, -speed, 0 });
		shader->setMat4("view", camera->getViewNatrix());
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera->move({ 0, speed, 0 });
		shader->setMat4("view", camera->getViewNatrix());
	}
}
