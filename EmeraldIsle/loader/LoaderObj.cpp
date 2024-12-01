//
// Created by apita on 01/12/2024.
//

#include "LoaderObj.h"

void LoaderObj::loadMTL(const std::string &path,std::unordered_map<std::string,Material> &materials) {
    std::cout << "MTL FILE LOADING " << path << std::endl;
    std::ifstream file(path);
    if (!file.is_open()) {
        throw "ERROR::MTLLOADER::Could not open file";
    }
    std::stringstream ss;
    std::string line;
    std::string prefix;
    glm::vec3 temp_diffuseColor;
    std::string temp_name;

    while (std::getline(file, line)) {
        ss.clear();
        ss.str(line);
        ss >> prefix;
        if (prefix == "newmtl") {
            if (!temp_name.empty()) {
                Material material;
                material.name = temp_name;
                material.colorDiffuse = temp_diffuseColor;
                materials[temp_name] = material;
            }
            ss >> temp_name;
        } else if (prefix == "Kd") {
            ss >> temp_diffuseColor.x >> temp_diffuseColor.y >> temp_diffuseColor.z;
        }
    }
    if (!temp_name.empty()) {
        Material material;
        material.name = temp_name;
        material.colorDiffuse = temp_diffuseColor;
        materials[temp_name] = material;
    }
    std::cout << "MTL Loaded successfully!" << std::endl;
}

void LoaderObj::loadOBJ(const std::string &pathObj,
                std::vector<GLfloat> & vertices,
                std::vector<GLfloat> & normals,
                std::vector<GLfloat> & colors,
                std::vector<GLuint> & indices,
                const std::string &pathMtl = "") {
    std::cout << "OBJ FILE LOADING " << pathObj << std::endl;

    std::unordered_map<std::string, Material> materials;
    bool fileMtl = !pathMtl.empty();
    if (fileMtl) {
        loadMTL(pathMtl,materials);
    }

    //Face vectors
    std::vector<GLfloat> temp_vertices;
    std::vector<GLfloat> temp_normals;

    std::stringstream ss;
    std::ifstream file(pathObj);
    std::string line;
    std::string prefix;
    glm::vec3 temp_vec3;
    GLuint temp_gluint;
    GLuint insertionIndex;
    std::string temp_str;
    glm::vec3 actualColor;

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

        } else if (prefix == "usemtl") {
            ss >> temp_str;
            if (fileMtl) {
                actualColor = materials[temp_str].colorDiffuse;
            } else {
                actualColor = glm::vec3(1.0f, 1.0f, 1.0f);
            }
        } else if (prefix == "v") {
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            temp_vertices.push_back(temp_vec3.x);
            temp_vertices.push_back(temp_vec3.y);
            temp_vertices.push_back(temp_vec3.z);
        } else if (prefix == "vt") {
            //if needed texcoord
        } else if (prefix == "vn") {
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            temp_normals.push_back(temp_vec3.x);
            temp_normals.push_back(temp_vec3.y);
            temp_normals.push_back(temp_vec3.z);
        } else if (prefix == "f") {
            int counter = 0;
            while (ss >> temp_gluint) {
                if (counter == 0) {
                    indices.push_back(temp_gluint-1);
                    insertionIndex = (temp_gluint-1)*3;
                    vertices[insertionIndex] = temp_vertices[insertionIndex];
                    vertices[insertionIndex+1] = temp_vertices[insertionIndex+1];
                    vertices[insertionIndex+2] = temp_vertices[insertionIndex+2];
                    colors[insertionIndex] = actualColor.x;
                    colors[insertionIndex+1] = actualColor.y;
                    colors[insertionIndex+2] = actualColor.z;
                } else if (counter == 1) {
                    // if needed texture
                } else if (counter == 2) {
                    normals[insertionIndex] = temp_normals[(temp_gluint-1)*3];
                    normals[insertionIndex+1] = temp_normals[(temp_gluint-1)*3+1];
                    normals[insertionIndex+2] = temp_normals[(temp_gluint-1)*3+2];
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
    for (GLuint i = 0; i < vertices.size(); i++) {
        std::cout << "normals[" << i << "] = " << vertices[i] << std::endl;
    }
}
