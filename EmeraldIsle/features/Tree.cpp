//
// Created by apita on 30/11/2024.
//

#include "Tree.h"

#include <loader/FileLoader.h>
#include <loader/FileLoader.h>

void Tree::initialize(glm::vec3 position, glm::vec3 scale, int n) {
    this->position = position;
    this->scale = scale;
    this->nInstances = n;

    vertices.resize(4104);
    normals.resize(4104);
    colors.resize(4104);
    FileLoader loader;
    loader.loadOBJ("../EmeraldIsle/model/pine.obj", vertices, normals,colors,indices,"../EmeraldIsle/model/pine.mtl");

    createModelMatrices(modelMatrices, nInstances);

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

    glGenBuffers(1, &matricesBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, matricesBufferID);
    glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), modelMatrices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    programID = LoadShadersFromFile("../EmeraldIsle/shader/tree.vert", "../EmeraldIsle/shader/tree.frag");
    if (programID == 0) {
        std::cerr << "Erreur : Impossible de charger les shaders 'tree'" << std::endl;
    }

    shadowProgramID = LoadShadersFromFile("../EmeraldIsle/shader/shadowMappingTree.vert", "../EmeraldIsle/shader/shadowMapping.frag");
    if (shadowProgramID == 0) {
        std::cerr << "Erreur : Impossible de charger les shaders 'shadow'" << std::endl;
    }

    vpMatrixID = glGetUniformLocation(programID, "VP");
    lightPositionID = glGetUniformLocation(programID, "lightPosition");
    lightIntensityID = glGetUniformLocation(programID, "lightIntensity");
    vpLightMatrixID = glGetUniformLocation(shadowProgramID, "VPLight");
    glBindVertexArray(0);
}

void Tree::createModelMatrices(std::vector<glm::mat4> & modelMatrices, int nInstances) {
    float radius = 6.0;
    float offset = 4.0;
    float rMaxOrigin = 40.0;
    float hMax = 1.0;
    srand(42);
    // Add the initial Tree
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, scale);
    modelMatrices.push_back(modelMatrix);
    // Add the others
    for (int i = 1; i < nInstances; i++) {
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        // Translation
        float angle = (float) i * 360.0f / nInstances;
        float displacement = (rand() % static_cast<int>(2 * offset * 100)) / 100.0f - offset;
        float x = position.x + sin(angle) * radius + displacement;
        float z = position.z + cos(angle) * radius + displacement;

        float distanceOrigin = std::sqrt(x * x + z * z);
        float y = distanceOrigin < rMaxOrigin ? rMaxOrigin * (1.0f - (distanceOrigin / rMaxOrigin)) : 0.0f;
        if (y > hMax) {
            y = hMax;
        }

        modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, z));

        // Scale
        modelMatrix = glm::scale(modelMatrix, scale);

        // Add to the list of matrices
        modelMatrices.push_back(modelMatrix);
    }
}

void Tree::render(glm::mat4 cameraMatrix, glm::mat4 lightMatrix) {
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

    glBindBuffer(GL_ARRAY_BUFFER, matricesBufferID);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(4 * sizeof(GLfloat)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(12 * sizeof(GLfloat)));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(glGetUniformLocation(programID,"shadowTexture"), 0);

    glUniformMatrix4fv(vpMatrixID, 1, GL_FALSE, &cameraMatrix[0][0]);

    glUniformMatrix4fv(glGetUniformLocation(programID, "VPLight"), 1, GL_FALSE, &lightMatrix[0][0]);

    // Set light data
    glUniform3fv(lightPositionID, 1, &lightPosition[0]);
    glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);

    glDrawElementsInstanced(
        GL_TRIANGLES,      // mode
        1368,              // number of indices
        GL_UNSIGNED_INT,   // type
        (void*)0,          // element array buffer offset
        nInstances         // number of instances
    );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);
    glDisableVertexAttribArray(6);
    glBindVertexArray(0);
}

void Tree::renderShadow(glm::mat4 lightMatrix) {
    glUseProgram(shadowProgramID);
    glBindVertexArray(vertexArrayID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, matricesBufferID);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(4 * sizeof(GLfloat)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(8 * sizeof(GLfloat)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(12 * sizeof(GLfloat)));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

    glUniformMatrix4fv(vpLightMatrixID, 1, GL_FALSE, &lightMatrix[0][0]);

    // Draw the box
    glDrawElementsInstanced(
        GL_TRIANGLES,      // mode
        1368,              // number of indices
        GL_UNSIGNED_INT,   // type
        (void*)0,          // element array buffer offset
        nInstances         // number of instances
    );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);
    glDisableVertexAttribArray(6);
    glBindVertexArray(0);
}

void Tree::cleanup() {
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &colorBufferID);
    glDeleteBuffers(1, &indexBufferID);
    glDeleteBuffers(1, &matricesBufferID);
    glDeleteVertexArrays(1, &normalBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(programID);
    glDeleteProgram(shadowProgramID);
}

