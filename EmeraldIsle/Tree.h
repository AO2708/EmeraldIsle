//
// Created by apita on 30/11/2024.
//

#ifndef TREE_H
#define TREE_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <render/shader.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
    glm::vec3 color;
};

struct Tree {
    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint indexBufferID;
    GLuint colorBufferID;
    GLuint normalBufferID;

    GLuint mvpMatrixID;
    GLuint mvpLightMatrixID;
    GLuint lightPositionID;
    GLuint lightIntensityID;
    GLuint programID;
    GLuint shadowProgramID;

    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> colors;
    std::vector<GLuint> indices;

    void initialize();

    void loadOBJ(const std::string &path,
                std::vector<GLfloat> &vertices,
                std::vector<GLfloat> &normals,
                std::vector<GLfloat> &colors,
                std::vector<GLuint> &indices);

    void render(glm::mat4);

    void cleanup();
};



#endif //TREE_H
