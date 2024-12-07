//
// Created by apita on 06/12/2024.
//

#ifndef ROBOT_H
#define ROBOT_H

#include <tiny_gltf.h>
#include "glad/gl.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <map>
#include <vector>
#include <render/shader.h>
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

extern const glm::vec3 lightIntensity;
extern const glm::vec3 lightPosition;
extern GLuint depthTexture;

struct Robot {
    glm::vec3 position;
    glm::vec3 scale;
    float rotation;

    // Attributes
    GLuint textureID;
    GLuint textureSamplerID;
    GLuint vpMatrixID;
    GLuint modelMatrixID;
    GLuint jointMatricesID;
    GLuint lightPositionID;
    GLuint lightIntensityID;
    GLuint programID;
    GLuint shadowProgramID;
    GLuint vpLightMatrixID;

    tinygltf::Model model;

    struct PrimitiveObject {
        GLuint vao;
        std::map<int, GLuint> vbos;
    };
    std::vector<PrimitiveObject> primitiveObjects;

    struct SkinObject {
        std::vector<glm::mat4> inverseBindMatrices;
        std::vector<glm::mat4> globalJointTransforms;
        std::vector<glm::mat4> jointMatrices;
    };
    std::vector<SkinObject> skinObjects;

    struct SamplerObject {
        std::vector<float> input;
        std::vector<glm::vec4> output;
        int interpolation;
    };

    struct ChannelObject {
        int sampler;
        std::string targetPath;
        int targetNode;
    };

    struct AnimationObject {
        std::vector<SamplerObject> samplers;	// Animation data
    };
    std::vector<AnimationObject> animationObjects;

    // Methods
    glm::mat4 getNodeTransform(const tinygltf::Node&);

    void computeLocalNodeTransform(const tinygltf::Model&,int,
        std::vector<glm::mat4>&);

    void computeGlobalNodeTransform(const tinygltf::Model&,
        const std::vector<glm::mat4> &, int, const glm::mat4&,
        std::vector<glm::mat4> &);

    std::vector<SkinObject> prepareSkinning(const tinygltf::Model &);

    int findKeyframeIndex(const std::vector<float>&, float);

    std::vector<AnimationObject> prepareAnimation(const tinygltf::Model &);

    void updateAnimation(const tinygltf::Model &, const tinygltf::Animation &,
        const AnimationObject &, float, std::vector<glm::mat4> &);

    void updateSkinning(const std::vector<glm::mat4> &);

    void update(float);

    bool loadModel(tinygltf::Model &, const char *);

    void initialize(glm::vec3, glm::vec3, float);

    void bindMesh(std::vector<PrimitiveObject> &, tinygltf::Model &, tinygltf::Mesh &);

    void bindModelNodes(std::vector<PrimitiveObject> &, tinygltf::Model &, tinygltf::Node &);

    std::vector<PrimitiveObject> bindModel(tinygltf::Model &);

    void drawMesh(const std::vector<PrimitiveObject> &, tinygltf::Model &,
        tinygltf::Mesh &);

    void drawModelNodes(const std::vector<PrimitiveObject>&, tinygltf::Model &,
        tinygltf::Node &);

    void drawModel(const std::vector<PrimitiveObject>&, tinygltf::Model &);

    void render(glm::mat4, glm::mat4);

    void renderShadow(glm::mat4);

    void cleanup();
};



#endif //ROBOT_H
