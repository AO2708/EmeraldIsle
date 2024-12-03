//
// Created by apita on 03/12/2024.
//

#ifndef PANNEL_H
#define PANNEL_H

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

struct Pannel {
    glm::vec3 position;
    glm::vec3 scale;
    float rotation;

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint indexBufferID;
    GLuint colorBufferID;
    GLuint normalBufferID;
    GLuint texCoordBufferID;
    GLuint isTexturedBufferID;

    GLuint textureID;
    GLuint textureSamplerID;
    GLuint vpMatrixID;
    GLuint modelMatrixID;
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

    void initialize(glm::vec3, glm::vec3, float rotation);

    void render(glm::mat4, glm::mat4);

    void renderShadow(glm::mat4);

    void cleanup();

};



#endif //PANNEL_H
