//
// Created by apita on 01/12/2024.
//

#include "FileHandler.h"
#include <stb_image.h>
#include <stb_image_write.h>

void FileHandler::saveDepthTexture(GLuint fbo, std::string filename, int width, int height) {
    int channels = 3;

    std::vector<float> depth(width * height);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glReadBuffer(GL_DEPTH_COMPONENT);
    glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depth.data());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::vector<unsigned char> img(width * height * 3);
    for (int i = 0; i < width * height; ++i) img[3*i] = img[3*i+1] = img[3*i+2] = depth[i] * 255;
    stbi_write_png(filename.c_str(), width, height, channels, img.data(), width * channels);
}

void FileHandler::loadMTL(const std::string &path,std::unordered_map<std::string,Material> &materials) {
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
    std::string temp_pathTexture;

    while (std::getline(file, line)) {
        ss.clear();
        ss.str(line);
        ss >> prefix;
        if (prefix == "newmtl") {
            if (!temp_name.empty()) {
                Material material;
                material.name = temp_name;
                material.colorDiffuse = temp_diffuseColor;
                if (!temp_pathTexture.empty()) {
                    material.texturePath = temp_pathTexture;
                    material.texture = true;
                } else {
                    material.texture = false;
                }
                materials[temp_name] = material;
                temp_pathTexture.clear();
            }
            ss >> temp_name;
        } else if (prefix == "Kd") {
            ss >> temp_diffuseColor.x >> temp_diffuseColor.y >> temp_diffuseColor.z;
        } else if (prefix == "map_Kd") {
            ss >> temp_pathTexture;
        }
    }
    if (!temp_name.empty()) {
        Material material;
        material.name = temp_name;
        material.colorDiffuse = temp_diffuseColor;
        if (!temp_pathTexture.empty()) {
            material.texturePath = temp_pathTexture;
            material.texture = true;
        } else {
            material.texture = false;
        }
        materials[temp_name] = material;
        temp_pathTexture.clear();
    }
    std::cout << "MTL Loaded successfully!" << std::endl;
}

void FileHandler::loadOBJ(const std::string &pathObj,
                std::vector<GLfloat> & vertices,
                std::vector<GLfloat> & normals,
                std::vector<GLfloat> & colors,
                std::vector<GLuint> & indices,
                const std::string &pathMtl) {
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
    int indexVertices = 0;

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
                    indices.push_back(indexVertices/3);
                    insertionIndex = (temp_gluint-1)*3;
                    vertices[indexVertices] = temp_vertices[insertionIndex];
                    vertices[indexVertices+1] = temp_vertices[insertionIndex+1];
                    vertices[indexVertices+2] = temp_vertices[insertionIndex+2];
                    colors[indexVertices] = actualColor.x;
                    colors[indexVertices+1] = actualColor.y;
                    colors[indexVertices+2] = actualColor.z;
                } else if (counter == 1) {
                    // if needed texture
                } else if (counter == 2) {
                    normals[indexVertices] = temp_normals[(temp_gluint-1)*3];
                    normals[indexVertices+1] = temp_normals[(temp_gluint-1)*3+1];
                    normals[indexVertices+2] = temp_normals[(temp_gluint-1)*3+2];
                    indexVertices = indexVertices+3;
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

void FileHandler::loadOBJ(const std::string &pathObj,
                std::vector<GLfloat> & vertices,
                std::vector<GLfloat> & normals,
                std::vector<GLfloat> & colors,
                std::vector<GLuint> & indices,
                std::vector<GLfloat> & texCoords,
                std::vector<GLuint> & isTexturedBuffer,
                const std::string &pathMtl) {
    std::cout << "OBJ FILE LOADING " << pathObj << std::endl;

    std::unordered_map<std::string, Material> materials;
    bool fileMtl = !pathMtl.empty();
    if (fileMtl) {
        loadMTL(pathMtl,materials);
    }

    //Face vectors
    std::vector<GLfloat> temp_vertices;
    std::vector<GLfloat> temp_normals;
    std::vector<GLfloat> temp_texCoords;

    std::stringstream ss;
    std::ifstream file(pathObj);
    std::string line;
    std::string prefix;
    glm::vec3 temp_vec3;
    glm::vec2 temp_vec2;
    GLuint temp_gluint;
    GLuint insertionIndex;
    std::string temp_str;
    glm::vec3 actualColor;
    bool isTextured;
    int indexVertices = 0;
    int indexTexCoords = 0;

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
                isTextured = materials[temp_str].texture;
                actualColor = materials[temp_str].colorDiffuse;
            } else {
                isTextured = false;
                actualColor = glm::vec3(1.0f, 1.0f, 1.0f);
            }
        } else if (prefix == "v") {
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            temp_vertices.push_back(temp_vec3.x);
            temp_vertices.push_back(temp_vec3.y);
            temp_vertices.push_back(temp_vec3.z);
        } else if (prefix == "vt") {
            ss >> temp_vec2.x >> temp_vec2.y;
            temp_texCoords.push_back(temp_vec2.x);
            temp_texCoords.push_back(temp_vec2.y);
        } else if (prefix == "vn") {
            ss >> temp_vec3.x >> temp_vec3.y >> temp_vec3.z;
            temp_normals.push_back(temp_vec3.x);
            temp_normals.push_back(temp_vec3.y);
            temp_normals.push_back(temp_vec3.z);
        } else if (prefix == "f") {
            int counter = 0;
            while (ss >> temp_gluint) {
                if (counter == 0) {
                    indices.push_back(indexVertices/3);
                    insertionIndex = (temp_gluint-1)*3;
                    vertices[indexVertices] = temp_vertices[insertionIndex];
                    vertices[indexVertices+1] = temp_vertices[insertionIndex+1];
                    vertices[indexVertices+2] = temp_vertices[insertionIndex+2];
                    colors[indexVertices] = actualColor.x;
                    colors[indexVertices+1] = actualColor.y;
                    colors[indexVertices+2] = actualColor.z;
                } else if (counter == 1) {
                    texCoords[indexTexCoords] = temp_texCoords[(temp_gluint-1)*2];
                    texCoords[indexTexCoords+1] = temp_texCoords[(temp_gluint-1)*2+1];
                    GLuint data = 0;
                    if (isTextured) {
                        data = 1;
                    }
                    isTexturedBuffer[indexVertices/3] = data;
                } else if (counter == 2) {
                    normals[indexVertices] = temp_normals[(temp_gluint-1)*3];
                    normals[indexVertices+1] = temp_normals[(temp_gluint-1)*3+1];
                    normals[indexVertices+2] = temp_normals[(temp_gluint-1)*3+2];
                    indexVertices = indexVertices+3;
                    indexTexCoords = indexTexCoords+2;
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

GLuint FileHandler::loadTextureTileBox(const char *texture_file_path) {
    int w, h, channels;
    uint8_t* img = stbi_load(texture_file_path, &w, &h, &channels, 3);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // To tile textures on a box, we set wrapping to repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (img) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture " << texture_file_path << std::endl;
    }
    stbi_image_free(img);

    return texture;
}
