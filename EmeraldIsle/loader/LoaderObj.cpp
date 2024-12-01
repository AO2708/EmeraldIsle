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
    std::vector<GLuint> vertex_normal_indicies;
    std::vector<GLuint> vertex_texcoord_indicies;

    std::stringstream ss;
    std::ifstream file(pathObj);
    std::string line;
    std::string prefix;
    glm::vec3 temp_vec3;
    glm::vec2 temp_vec2;
    GLuint temp_gluint;
    std::string temp_str;
    int counterVertex = 0;

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
                temp_vec3 = materials[temp_str].colorDiffuse;
            } else {
                temp_vec3 = glm::vec3(1.0f, 1.0f, 1.0f);
            }
            for (int i = 0; i < counterVertex; i++) {
                colors.push_back(temp_vec3.x);
                colors.push_back(temp_vec3.y);
                colors.push_back(temp_vec3.z);
            }
            counterVertex = 0;
        } else if (prefix == "v") {
            counterVertex++;
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            vertices.push_back(temp_vec3.x);
            vertices.push_back(temp_vec3.y);
            vertices.push_back(temp_vec3.z);
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
}
