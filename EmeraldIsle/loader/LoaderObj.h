//
// Created by apita on 01/12/2024.
//

#ifndef LOADEROBJ_H
#define LOADEROBJ_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <render/shader.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>


struct Material {
    glm::vec3 colorDiffuse;
    std::string name;
};

struct LoaderObj {

    void loadMTL(const std::string &, std::unordered_map<std::string, Material> &materials);

    void loadOBJ(const std::string &pathObj, std::vector<GLfloat> &vertices, std::vector<GLfloat> &normals,
                 std::vector<GLfloat> &colors, std::vector<GLuint> &indices, const std::string &pathMtl);

};



#endif //LOADEROBJ_H
