#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Model.h"
#include "Shader.h"

class Object {
private:
	Model model;
	glm::vec3 position, rotation, scaling;
public:
	Object(const Model& m, glm::vec3 pos, glm::vec3 rot, glm::vec3 sc);
	Object(std::string filePath, glm::vec3 pos, glm::vec3 rot, glm::vec3 sc);

	void render(const Shader& shader);

	glm::mat4 getModelMatrix() const;

	inline void move(glm::vec3 vec) { position += vec; }
	inline void rotate(glm::vec3 vec) { rotation += vec; }
	inline void scale(glm::vec3 vec) { scaling += vec; }

	inline glm::vec3 getPosition() const { return position; }
	inline glm::vec3 getRotation() const { return rotation; }
	inline glm::vec3 getScale() const	 { return scaling; }

	inline Model getModel() const { return model; }
};

Object::Object(const Model& m, glm::vec3 pos, glm::vec3 rot = glm::vec3(0.0), glm::vec3 sc = glm::vec3(0.0)): model(m), position(pos), rotation(rot), scaling(sc) {}

Object::Object(std::string filePath, glm::vec3 pos, glm::vec3 rot = glm::vec3(0.0), glm::vec3 sc = glm::vec3(1.0)) : position(pos), rotation(rot), scaling(sc) {
	model.loadModel(filePath);
}

void Object::render(const Shader& shader) {
	shader.use();
	shader.setMat4("model", getModelMatrix());
	model.render(shader);
}

glm::mat4 Object::getModelMatrix() const {
	glm::mat4 res = glm::mat4();

	res = glm::translate(res, position);
	res = glm::rotate(res, rotation.x, { 1.0, 0.0, 0.0 });
	res = glm::rotate(res, rotation.y, { 0.0, 1.0, 0.0 });
	res = glm::rotate(res, rotation.z, { 0.0, 0.0, 1.0 });
	res = glm::scale(res, scaling);

	return res;
}
