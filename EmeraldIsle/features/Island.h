//
// Created by apita on 02/12/2024.
//

#ifndef ISLAND_H
#define ISLAND_H

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

struct Island {
    glm::vec3 position;
    glm::vec3 scale;

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint indexBufferID;
    GLuint colorBufferID;
    GLuint normalBufferID;
    GLuint texCoordBufferID;
    GLuint isTexturedBufferID;

    GLuint textureID;
    GLuint textureSamplerID;
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
    std::vector<GLfloat> textures;
    std::vector<GLuint> isTextured;

    void initialize(glm::vec3, glm::vec3);

    void render(glm::mat4, glm::mat4);

    void renderShadow(glm::mat4);

    void cleanup();
};


#endif //ISLAND_H
