//
// Created by apita on 05/12/2024.
//

#include "Sea.h"

void Sea::initialize(glm::vec3 position, glm::vec3 scale) {
    this->position = position;
    this->scale = scale;

    std::vector<GLfloat> colors;
    std::vector<GLuint> isTextured;
    vertices.resize(18432);
    normals.resize(18432);
    textures.resize(12288);
    colors.resize(18432);
    isTextured.resize(6144);
    FileLoader loader;
    loader.loadOBJ("../EmeraldIsle/model/sea.obj", vertices, normals, colors, indices, textures, isTextured);
    colors.clear();
    colors.shrink_to_fit();
    isTextured.clear();
    isTextured.shrink_to_fit();

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &normalBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &texCoordBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBufferID);
    glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(GLfloat), textures.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    programID = LoadShadersFromFile("../EmeraldIsle/shader/sea.vert", "../EmeraldIsle/shader/sea.frag");
    if (programID == 0) {
        std::cerr << "Erreur : Impossible de charger les shaders 'sea'" << std::endl;
    }

    // Load a Texture
    textureID = loader.loadTextureTileBox("../EmeraldIsle/texture/normalMappingSea.jpg");
    textureSamplerID = glGetUniformLocation(programID, "normalMap");

    vpMatrixID = glGetUniformLocation(programID, "VP");
    modelMatrixID = glGetUniformLocation(programID, "modelMatrix");
    mvpLightMatrixID = glGetUniformLocation(programID, "MVPLight");
    lightPositionID = glGetUniformLocation(programID, "lightPosition");
    lightIntensityID = glGetUniformLocation(programID, "lightIntensity");
    eyePositionID = glGetUniformLocation(programID, "cameraPosition");
    timeID = glGetUniformLocation(programID, "time");
    glBindVertexArray(0);
}

void Sea::render(glm::mat4 cameraMatrix, glm::mat4 lightMatrix) {
    glUseProgram(programID);
    glBindVertexArray(vertexArrayID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBufferID);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureSamplerID, 0);

    glm::mat4 modelMatrix = glm::mat4();
    // Scale the box along each axis to make it look like a building
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);

    glUniformMatrix4fv(vpMatrixID, 1, GL_FALSE, &cameraMatrix[0][0]);
    glUniformMatrix4fv(modelMatrixID, 1, GL_FALSE, &modelMatrix[0][0]);

    glm::mat4 mvpLight = lightMatrix * modelMatrix;
    glUniformMatrix4fv(glGetUniformLocation(programID, "MVPLight"), 1, GL_FALSE, &mvpLight[0][0]);

    // Set light data
    glUniform3fv(lightPositionID, 1, &lightPosition[0]);
    glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);
    glUniform3fv(eyePositionID, 1, &eye_center[0]);
    float time = glfwGetTime();
    glUniform1f(timeID, time);

    glDrawElements(
        GL_TRIANGLES,      // mode
        6144,              // number of indices
        GL_UNSIGNED_INT,   // type
        (void*)0           // element array buffer offset
    );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindVertexArray(0);
}

void Sea::cleanup() {
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &indexBufferID);
    glDeleteVertexArrays(1, &normalBufferID);
    glDeleteVertexArrays(1, &texCoordBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(programID);
    //glDeleteProgram(shadowProgramID);
}
