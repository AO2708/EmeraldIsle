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
#include <loader/LoaderObj.h>

extern const glm::vec3 lightPosition;
extern const glm::vec3 lightIntensity;
extern GLuint depthTexture;

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

    void render(glm::mat4, glm::mat4);

    void renderShadow(glm::mat4);

    void cleanup();
};



#endif //TREE_H
