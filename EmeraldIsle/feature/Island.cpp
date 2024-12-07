//
// Created by apita on 02/12/2024.
//

#include "Island.h"

void Island::initialize(glm::vec3 position, glm::vec3 scale) {
    this->position = position;
    this->scale = scale;

    vertices.resize(17298);
    normals.resize(17298);
    colors.resize(17298);
    textures.resize(11532);
    isTextured.resize(5766);
    FileHandler loader;
    loader.loadOBJ("../EmeraldIsle/model/island.obj", vertices, normals,colors,indices, textures, isTextured,"../EmeraldIsle/model/island.mtl");

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &colorBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), colors.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &normalBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), normals.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &texCoordBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBufferID);
    glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(GLfloat), textures.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &isTexturedBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, isTexturedBufferID);
    glBufferData(GL_ARRAY_BUFFER, isTextured.size() * sizeof(GLuint), isTextured.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    programID = LoadShadersFromFile("../EmeraldIsle/shader/island.vert", "../EmeraldIsle/shader/island.frag");
    if (programID == 0) {
        std::cerr << "Erreur : Impossible de charger les shaders 'island'" << std::endl;
    }

    shadowProgramID = LoadShadersFromFile("../EmeraldIsle/shader/shadowMapping.vert", "../EmeraldIsle/shader/shadowMapping.frag");
    if (shadowProgramID == 0) {
        std::cerr << "Erreur : Impossible de charger les shaders 'shadow'" << std::endl;
    }

    // Load a Texture
    textureID = loader.loadTextureTileBox("../EmeraldIsle/texture/sand.jpg");
    textureSamplerID = glGetUniformLocation(programID, "textureSampler");

    mvpMatrixID = glGetUniformLocation(programID, "MVP");
    lightPositionID = glGetUniformLocation(programID, "lightPosition");
    lightIntensityID = glGetUniformLocation(programID, "lightIntensity");
    mvpLightMatrixID = glGetUniformLocation(shadowProgramID, "MVPLight");
    glBindVertexArray(0);
}

void Island::render(glm::mat4 cameraMatrix, glm::mat4 lightMatrix) {
    glUseProgram(programID);
    glBindVertexArray(vertexArrayID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBufferID);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, isTexturedBufferID);
    glVertexAttribPointer(4, 1, GL_UNSIGNED_INT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(glGetUniformLocation(programID,"shadowTexture"), 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureSamplerID, 0);

    glm::mat4 modelMatrix = glm::mat4();
    // Scale the box along each axis to make it look like a building
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);

    glm::mat4 mvp = cameraMatrix * modelMatrix;
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

    glm::mat4 mvpLight = lightMatrix * modelMatrix;
    glUniformMatrix4fv(glGetUniformLocation(programID, "MVPLight"), 1, GL_FALSE, &mvpLight[0][0]);

    // Set light data
    glUniform3fv(lightPositionID, 1, &lightPosition[0]);
    glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);

    glDrawElements(
        GL_TRIANGLES,      // mode
        5766,              // number of indices
        GL_UNSIGNED_INT,   // type
        (void*)0           // element array buffer offset
    );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glBindVertexArray(0);
}

void Island::renderShadow(glm::mat4 lightMatrix) {
    glUseProgram(shadowProgramID);
    glBindVertexArray(vertexArrayID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

    // Set model-view-projection matrix
    glm::mat4 modelMatrix = glm::mat4();
    // Scale the box along each axis to make it look like a building
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);
    glm::mat4 mvp = lightMatrix * modelMatrix;
    glUniformMatrix4fv(mvpLightMatrixID, 1, GL_FALSE, &mvp[0][0]);

    // Draw the box
    glDrawElements(
        GL_TRIANGLES,      // mode
        1368,    			   // number of indices
        GL_UNSIGNED_INT,   // type
        (void*)0           // element array buffer offset
    );

    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
}

void Island::cleanup() {
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &colorBufferID);
    glDeleteBuffers(1, &indexBufferID);
    glDeleteBuffers(1, &normalBufferID);
    glDeleteBuffers(1, &texCoordBufferID);
    glDeleteBuffers(1, &isTexturedBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(programID);
    glDeleteProgram(shadowProgramID);
}