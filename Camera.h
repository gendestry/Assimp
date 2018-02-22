#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum MoveDir {
	FORWARD,
	BACKWARD,
	UP,
	DOWN,
	LEFT,
	RIGHT
};

class Camera {
private:
	glm::mat4 view;
	glm::vec3 position;
	glm::vec3 rotation;

	void updateMatrix();
public:
	Camera(glm::vec3 pos, glm::vec3 rot = glm::vec3(0.0)) : position(pos), rotation(rot){ updateMatrix(); }

	void move(MoveDir dir, float speed);
	void rotate(glm::vec3 rot);

	inline glm::mat4 getViewNatrix() const { return view; }
	inline glm::vec3 getPosition() const { return position; }
};

void Camera::updateMatrix() {
	view = glm::mat4(1.0f);
	view = glm::rotate(view, glm::radians(rotation.y), { 1.0f, 0.0f, 0.0f }); // pitch
	view = glm::rotate(view, glm::radians(rotation.x), { 0.0f, 1.0f, 0.0f }); // yaw
	view = glm::translate(view, -position);
}

void Camera::move(MoveDir dir, float speed) {
	glm::vec3 pos{0.0f};

	if (dir == FORWARD)
		pos.z -= speed;
	else if (dir == BACKWARD)
		pos.z = speed;

	else if (dir == UP)
		pos.y = speed;
	else if (dir == DOWN)
		pos.y -= speed;

	else if (dir == LEFT)
		pos.x -= speed;
	else if (dir == RIGHT)
		pos.x = speed;

	glm::vec3 temp;
	temp.x = cos(glm::radians(rotation.x)) * pos.x - sin(glm::radians(rotation.x)) * pos.z * cos(glm::radians(rotation.y)) + pos.y * sin(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
	temp.y = sin(glm::radians(rotation.y)) * pos.z + pos.y * cos(glm::radians(rotation.y));
	temp.z = sin(glm::radians(rotation.x)) * pos.x + cos(glm::radians(rotation.x)) * pos.z * cos(glm::radians(rotation.y)) - pos.y * sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
	position += temp;
	updateMatrix();
}

void Camera::rotate(glm::vec3 rot) {
	rotation += rot;
	if (rotation.y > 90.0f) rotation.y = 90.0f;
	if (rotation.y < -90.0f) rotation.y = -90.0f;
	updateMatrix();
}