//
// Created by apita on 05/12/2024.
//

#ifndef SEA_H
#define SEA_H
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <render/shader.h>

#include <vector>
#include <loader/FileLoader.h>

extern const glm::vec3 lightPosition;
extern const glm::vec3 lightIntensity;
extern glm::vec3 eye_center;

struct Sea {
    glm::vec3 position;
    glm::vec3 scale;

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint normalBufferID;
    GLuint texCoordBufferID;
    GLuint indexBufferID;

    GLuint textureID;
    GLuint textureSamplerID;
    GLuint vpMatrixID;
    GLuint modelMatrixID;
    GLuint mvpLightMatrixID;
    GLuint lightPositionID;
    GLuint lightIntensityID;
    GLuint eyePositionID;
    GLuint timeID;
    GLuint programID;

    std::vector<GLfloat> vertices;
    std::vector<GLfloat> normals;
    std::vector<GLfloat> textures;
    std::vector<GLuint> indices;

    void initialize(glm::vec3, glm::vec3);

    void render(glm::mat4, glm::mat4);

    void cleanup();
};



#endif //SEA_H
