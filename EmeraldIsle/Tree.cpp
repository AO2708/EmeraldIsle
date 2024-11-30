//
// Created by apita on 30/11/2024.
//

#include "Tree.h"

void Tree::initialize() {
    loadOBJ("../EmeraldIsle/model/pine.obj", vertices, normals,colors,indices);

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &colorBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), colors.data(), GL_STATIC_DRAW);
    std::cout << sizeof(colors) << std::endl;

    /*glGenBuffers(1, &normalBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals.data(), GL_STATIC_DRAW);
    */
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    std::cout << sizeof(indices) << std::endl;

    programID = LoadShadersFromFile("../EmeraldIsle/shader/tree.vert", "../EmeraldIsle/shader/tree.frag");
    if (programID == 0) {
        std::cerr << "Erreur : Impossible de charger les shaders 'tree'" << std::endl;
    }

    mvpMatrixID = glGetUniformLocation(programID, "MVP");
}

void Tree::loadOBJ(const std::string &path,
                std::vector<GLfloat> & vertices,
                std::vector<GLfloat> & normals,
                std::vector<GLfloat> & colors,
                std::vector<GLuint> & indices) {
    std::cout << "OBJ FILE LOADING " << path << std::endl;

    //Face vectors
    std::vector<GLuint> vertex_normal_indicies;
    std::vector<GLuint> vertex_texcoord_indicies;

    std::stringstream ss;
    std::ifstream file(path);
    std::string line;
    std::string prefix;
    glm::vec3 temp_vec3;
    glm::vec2 temp_vec2;
    GLuint temp_gluint;

    if (!file.is_open()) {
        throw "ERROR::OBJLOADER::Could not open file";
    }

    while (getline(file, line)) {
        // Get the prefix of the line
        ss.clear();
        ss.str(line);
        ss >> prefix;

        if (prefix == "#") {

        } else if (prefix == "o") {

        } else if (prefix == "s") {

        } else if (prefix == "use_mtl") {

        } else if (prefix == "v") {
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            vertices.push_back(temp_vec3.x);
            vertices.push_back(temp_vec3.y);
            vertices.push_back(temp_vec3.z);
            colors.emplace_back(1.0f);
            colors.emplace_back(1.0f);
            colors.emplace_back(1.0f);
        } else if (prefix == "vt") {
            ss >> temp_vec2.x >> temp_vec2.y;
            //if needed texcoord
        } else if (prefix == "vn") {
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            normals.push_back(temp_vec3.x);
            normals.push_back(temp_vec3.y);
            normals.push_back(temp_vec3.z);
        } else if (prefix == "f") {
            int counter = 0;
            while (ss >> temp_gluint) {
                if (counter == 0) {
                    indices.push_back(temp_gluint-1);
                } else if (counter == 1) {
                    vertex_texcoord_indicies.push_back(temp_gluint);
                } else if (counter == 2) {
                    vertex_normal_indicies.push_back(temp_gluint);
                }
                if (ss.peek() == '/') {
                    ++counter;
                    ss.ignore(1,'/');
                } else if (ss.peek() == ' ') {
                    ++counter;
                    ss.ignore(1,' ');
                }
                // Reset the counter
                if (counter > 2) {
                    counter = 0;
                }
            }
        } else {

        }
    }
    std::cout << "OBJ FILE LOADED" << std::endl;
    std::cout << "Vertices: " << vertices.size() << std::endl;
    std::cout << "Normals: " << normals.size() << std::endl;
    std::cout << "Colors: " << colors.size() << std::endl;
    std::cout << "Indices: " << indices.size() << std::endl;
}


void Tree::render(glm::mat4 cameraMatrix) {
    glUseProgram(programID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    /*glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

    glm::mat4 mvp = cameraMatrix;
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

    glDrawElements(
        GL_TRIANGLES,      // mode
        1368,              // number of indices
        GL_UNSIGNED_INT,   // type
        (void*)0           // element array buffer offset
    );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    //glDisableVertexAttribArray(2);
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

