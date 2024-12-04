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
#include <loader/FileLoader.h>

extern const glm::vec3 lightPosition;
extern const glm::vec3 lightIntensity;
extern GLuint depthTexture;

struct Forest {
    glm::vec3 position;
    glm::vec3 scale;
    int nInstances;

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint indexBufferID;
    GLuint colorBufferID;
    GLuint matricesBufferID;
    GLuint normalBufferID;

    GLuint vpMatrixID;
    GLuint vpLightMatrixID;
    GLuint lightPositionID;
    GLuint lightIntensityID;
    GLuint programID;
    GLuint shadowProgramID;

    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> colors;
    std::vector<GLuint> indices;
    std::vector <glm::mat4> modelMatrices;

    void initialize(glm::vec3, glm::vec3, int);

    void createModelMatrices(std::vector<glm::mat4> &, int);

    void render(glm::mat4, glm::mat4);

    void renderShadow(glm::mat4);

    void cleanup();
};



#endif //TREE_H
