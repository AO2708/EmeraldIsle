//
// Created by apita on 30/11/2024.
//

#include "Tree.h"

void Tree::initialize() {
    vertices.resize(4104);
    normals.resize(4104);
    colors.resize(4104);
    LoaderObj loader;
    loader.loadOBJ("../EmeraldIsle/model/pine.obj", vertices, normals,colors,indices,"../EmeraldIsle/model/pine.mtl");

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

    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    programID = LoadShadersFromFile("../EmeraldIsle/shader/tree.vert", "../EmeraldIsle/shader/tree.frag");
    if (programID == 0) {
        std::cerr << "Erreur : Impossible de charger les shaders 'tree'" << std::endl;
    }

    mvpMatrixID = glGetUniformLocation(programID, "MVP");
    lightPositionID = glGetUniformLocation(programID, "lightPosition");
    lightIntensityID = glGetUniformLocation(programID, "lightIntensity");
}

void Tree::render(glm::mat4 cameraMatrix) {
    glUseProgram(programID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

    glm::mat4 mvp = cameraMatrix;
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

    // Set light data
    glUniform3fv(lightPositionID, 1, &lightPosition[0]);
    glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);

    glDrawElements(
        GL_TRIANGLES,      // mode
        1368,              // number of indices
        GL_UNSIGNED_INT,   // type
        (void*)0           // element array buffer offset
    );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Tree::cleanup() {
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &colorBufferID);
    glDeleteBuffers(1, &indexBufferID);
    glDeleteVertexArrays(1, &normalBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteProgram(programID);
    glDeleteProgram(shadowProgramID);
}

