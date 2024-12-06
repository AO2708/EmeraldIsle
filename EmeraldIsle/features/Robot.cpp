//
// Created by apita on 06/12/2024.
//

#include "Robot.h"

glm::mat4 Robot::getNodeTransform(const tinygltf::Node& node) {
	glm::mat4 transform(1.0f);

	if (node.matrix.size() == 16) {
		transform = glm::make_mat4(node.matrix.data());
	} else {
		if (node.translation.size() == 3) {
			transform = glm::translate(transform, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
		}
		if (node.rotation.size() == 4) {
			glm::quat q(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
			transform *= glm::mat4_cast(q);
		}
		if (node.scale.size() == 3) {
			transform = glm::scale(transform, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
		}
	}
	return transform;
}

void Robot::computeLocalNodeTransform(const tinygltf::Model& model,
	int nodeIndex,
	std::vector<glm::mat4> &localTransforms)
{
	const tinygltf::Node & node = model.nodes[nodeIndex];
	localTransforms[nodeIndex] = getNodeTransform(node);
	if(!node.children.empty()) {
		for(int childIndex : node.children) {
			computeLocalNodeTransform(model, childIndex, localTransforms);
		}
	}
}

void Robot::computeGlobalNodeTransform(const tinygltf::Model& model,
	const std::vector<glm::mat4> &localTransforms,
	int nodeIndex, const glm::mat4& parentTransform,
	std::vector<glm::mat4> &globalTransforms)
{
	const tinygltf::Node & node = model.nodes[nodeIndex];
	globalTransforms[nodeIndex] = parentTransform * localTransforms[nodeIndex];
	if(!node.children.empty()) {
		for(int childIndex : node.children) {
			computeGlobalNodeTransform(model, localTransforms, childIndex, globalTransforms[nodeIndex],globalTransforms);
		}
	}
}

std::vector<Robot::SkinObject> Robot::prepareSkinning(const tinygltf::Model &model) {
	std::vector<SkinObject> skinObjects;

	// In our Blender exporter, the default number of joints that may influence a vertex is set to 4, just for convenient implementation in shaders.

	for (size_t i = 0; i < model.skins.size(); i++) {
		SkinObject skinObject;

		const tinygltf::Skin &skin = model.skins[i];

		// Read inverseBindMatrices
		const tinygltf::Accessor &accessor = model.accessors[skin.inverseBindMatrices];
		assert(accessor.type == TINYGLTF_TYPE_MAT4);
		const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
		const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
		const float *ptr = reinterpret_cast<const float *>(
            buffer.data.data() + accessor.byteOffset + bufferView.byteOffset);

		skinObject.inverseBindMatrices.resize(accessor.count);
		for (size_t j = 0; j < accessor.count; j++) {
			float m[16];
			memcpy(m, ptr + j * 16, 16 * sizeof(float));
			skinObject.inverseBindMatrices[j] = glm::make_mat4(m);
		}

		assert(skin.joints.size() == accessor.count);

		skinObject.globalJointTransforms.resize(skin.joints.size());
		skinObject.jointMatrices.resize(skin.joints.size());

		// ----------------------------------------------
		// your code here to compute joint matrices
		// ----------------------------------------------
		// Compute local transforms at each node
		int rootNodeIndex = skin.joints[0];
		std::vector<glm::mat4> localNodeTransforms(skin.joints.size());
		computeLocalNodeTransform(model, rootNodeIndex, localNodeTransforms);

		// Compute global transforms at each node
		glm::mat4 parentTransform(1.0f);
		computeGlobalNodeTransform(model, localNodeTransforms, rootNodeIndex, parentTransform, skinObject.globalJointTransforms);

		// Compute the inverseBindMatrix
		for(int j = 0; j < skinObject.jointMatrices.size(); j++) {
			int nodeIndex = skin.joints[j];
			skinObject.jointMatrices[j] = skinObject.globalJointTransforms[nodeIndex] * skinObject.inverseBindMatrices[j];
		}
		// ----------------------------------------------

		skinObjects.push_back(skinObject);
	}
	return skinObjects;
}

int Robot::findKeyframeIndex(const std::vector<float>& times, float animationTime)
{
	int left = 0;
	int right = times.size() - 1;

	while (left <= right) {
		int mid = (left + right) / 2;

		if (mid + 1 < times.size() && times[mid] <= animationTime && animationTime < times[mid + 1]) {
			return mid;
		}
		else if (times[mid] > animationTime) {
			right = mid - 1;
		}
		else { // animationTime >= times[mid + 1]
			left = mid + 1;
		}
	}

	// Target not found
	return times.size() - 2;
}

std::vector<Robot::AnimationObject> Robot::prepareAnimation(const tinygltf::Model &model)
{
	std::vector<AnimationObject> animationObjects;
	for (const auto &anim : model.animations) {
		AnimationObject animationObject;

		for (const auto &sampler : anim.samplers) {
			SamplerObject samplerObject;

			const tinygltf::Accessor &inputAccessor = model.accessors[sampler.input];
			const tinygltf::BufferView &inputBufferView = model.bufferViews[inputAccessor.bufferView];
			const tinygltf::Buffer &inputBuffer = model.buffers[inputBufferView.buffer];

			assert(inputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			assert(inputAccessor.type == TINYGLTF_TYPE_SCALAR);

			// Input (time) values
			samplerObject.input.resize(inputAccessor.count);

			const unsigned char *inputPtr = &inputBuffer.data[inputBufferView.byteOffset + inputAccessor.byteOffset];
			const float *inputBuf = reinterpret_cast<const float*>(inputPtr);

			// Read input (time) values
			int stride = inputAccessor.ByteStride(inputBufferView);
			for (size_t i = 0; i < inputAccessor.count; ++i) {
				samplerObject.input[i] = *reinterpret_cast<const float*>(inputPtr + i * stride);
			}

			const tinygltf::Accessor &outputAccessor = model.accessors[sampler.output];
			const tinygltf::BufferView &outputBufferView = model.bufferViews[outputAccessor.bufferView];
			const tinygltf::Buffer &outputBuffer = model.buffers[outputBufferView.buffer];

			assert(outputAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

			const unsigned char *outputPtr = &outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset];
			const float *outputBuf = reinterpret_cast<const float*>(outputPtr);

			int outputStride = outputAccessor.ByteStride(outputBufferView);

			// Output values
			samplerObject.output.resize(outputAccessor.count);

			for (size_t i = 0; i < outputAccessor.count; ++i) {

				if (outputAccessor.type == TINYGLTF_TYPE_VEC3) {
					memcpy(&samplerObject.output[i], outputPtr + i * 3 * sizeof(float), 3 * sizeof(float));
				} else if (outputAccessor.type == TINYGLTF_TYPE_VEC4) {
					memcpy(&samplerObject.output[i], outputPtr + i * 4 * sizeof(float), 4 * sizeof(float));
				} else {
					std::cout << "Unsupport accessor type ..." << std::endl;
				}

			}

			animationObject.samplers.push_back(samplerObject);
		}

		animationObjects.push_back(animationObject);
	}
	return animationObjects;
}

void Robot::updateAnimation(
	const tinygltf::Model &model,
	const tinygltf::Animation &anim,
	const AnimationObject &animationObject,
	float time,
	std::vector<glm::mat4> &nodeTransforms)
{
	// There are many channels so we have to accumulate the transforms
	for (const auto &channel : anim.channels) {

		int targetNodeIndex = channel.target_node;
		const auto &sampler = anim.samplers[channel.sampler];

		// Access output (value) data for the channel
		const tinygltf::Accessor &outputAccessor = model.accessors[sampler.output];
		const tinygltf::BufferView &outputBufferView = model.bufferViews[outputAccessor.bufferView];
		const tinygltf::Buffer &outputBuffer = model.buffers[outputBufferView.buffer];

		// Calculate current animation time (wrap if necessary)
		const std::vector<float> &times = animationObject.samplers[channel.sampler].input;
		float animationTime = fmod(time, times.back());

		// ----------------------------------------------------------
		// Find a keyframe for getting animation data
		// ----------------------------------------------------------
		int keyframeIndex = findKeyframeIndex(times,animationTime);

		const unsigned char *outputPtr = &outputBuffer.data[outputBufferView.byteOffset + outputAccessor.byteOffset];
		const float *outputBuf = reinterpret_cast<const float*>(outputPtr);

		// -----------------------------------------------------------
		// Add interpolation for smooth animation
		// -----------------------------------------------------------
		if (channel.target_path == "translation") {
			glm::vec3 translation0, translation1;
			memcpy(&translation0, outputPtr + keyframeIndex * 3 * sizeof(float), 3 * sizeof(float));
			memcpy(&translation1, outputPtr + (keyframeIndex+1) * 3 * sizeof(float), 3 * sizeof(float));

			float t = (animationTime - times[keyframeIndex]) / (times[keyframeIndex + 1] - times[keyframeIndex]);
			glm::vec3 translation = translation0 + t * (translation1 - translation0);
			nodeTransforms[targetNodeIndex] = glm::translate(nodeTransforms[targetNodeIndex], translation);
		} else if (channel.target_path == "rotation") {
			glm::quat rotation0, rotation1;
			memcpy(&rotation0, outputPtr + keyframeIndex * 4 * sizeof(float), 4 * sizeof(float));
			memcpy(&rotation1, outputPtr + (keyframeIndex+1) * 4 * sizeof(float), 4 * sizeof(float));

			float t = (animationTime - times[keyframeIndex]) / (times[keyframeIndex + 1] - times[keyframeIndex]);
			glm::quat rotation = glm::slerp(rotation0, rotation1, t);
			nodeTransforms[targetNodeIndex] *= glm::mat4_cast(rotation);
		} else if (channel.target_path == "scale") {
			glm::vec3 scale0, scale1;
			memcpy(&scale0, outputPtr + keyframeIndex * 3 * sizeof(float), 3 * sizeof(float));
			memcpy(&scale1, outputPtr + (keyframeIndex+1) * 3 * sizeof(float), 3 * sizeof(float));

			float t = (animationTime - times[keyframeIndex]) / (times[keyframeIndex + 1] - times[keyframeIndex]);
			glm::vec3 scale = scale0 + t * (scale1 - scale0);
			nodeTransforms[targetNodeIndex] = glm::scale(nodeTransforms[targetNodeIndex], scale);
		}
	}
}

void Robot::updateSkinning(const std::vector<glm::mat4> &nodeTransforms) {
	// -------------------------------------------------
	// Recompute joint matrices
	// -------------------------------------------------
	for (size_t i = 0; i < model.skins.size(); i++) {
		SkinObject & skinObject = skinObjects[i];

		const tinygltf::Skin &skin = model.skins[i];

		// Compute global transforms at each node
		int rootNodeIndex = skin.joints[0];
		glm::mat4 parentTransform(1.0f);
		computeGlobalNodeTransform(model, nodeTransforms, rootNodeIndex, parentTransform, skinObject.globalJointTransforms);

		// Compute the inverseBindMatrix
		for(int j = 0; j < skinObject.jointMatrices.size(); j++) {
			int nodeIndex = skin.joints[j];
			skinObject.jointMatrices[j] = skinObject.globalJointTransforms[nodeIndex] * skinObject.inverseBindMatrices[j];
		}
		// ----------------------------------------------
	}
}

void Robot::update(float time) {
	// Update Animations
	if (model.animations.size() > 0) {
		const tinygltf::Animation &animation = model.animations[0];
		const AnimationObject &animationObject = animationObjects[0];

		const tinygltf::Skin &skin = model.skins[0];
		std::vector<glm::mat4> nodeTransforms(skin.joints.size());
		for (size_t i = 0; i < nodeTransforms.size(); ++i) {
			nodeTransforms[i] = glm::mat4(1.0);
		}

		updateAnimation(model, animation, animationObject, time, nodeTransforms);

		updateSkinning(nodeTransforms);
	}
}

bool Robot::loadModel(tinygltf::Model &model, const char *filename) {
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cout << "ERR: " << err << std::endl;
	}

	if (!res)
		std::cout << "Failed to load glTF: " << filename << std::endl;
	else
		std::cout << "Loaded glTF: " << filename << std::endl;

	return res;
}

void Robot::initialize() {
	if (!loadModel(model, "../EmeraldIsle/model/robot.gltf")) {
		return;
	}

	// Prepare buffers for rendering
	primitiveObjects = bindModel(model);

	// Prepare joint matrices
	skinObjects = prepareSkinning(model);

	// Prepare animation data
	animationObjects = prepareAnimation(model);

	// Create and compile our GLSL program from the shaders
	programID = LoadShadersFromFile("../EmeraldIsle/shader/robot.vert", "../EmeraldIsle/shader/robot.frag");
	if (programID == 0)
	{
		std::cerr << "Failed to load shaders." << std::endl;
	}

	// Get a handle for GLSL variables
	mvpMatrixID = glGetUniformLocation(programID, "MVP");
	lightPositionID = glGetUniformLocation(programID, "lightPosition");
	lightIntensityID = glGetUniformLocation(programID, "lightIntensity");
}

void Robot::bindMesh(std::vector<PrimitiveObject> &primitiveObjects,
			tinygltf::Model &model, tinygltf::Mesh &mesh) {

	std::map<int, GLuint> vbos;
	for (size_t i = 0; i < model.bufferViews.size(); ++i) {
		const tinygltf::BufferView &bufferView = model.bufferViews[i];

		int target = bufferView.target;

		if (bufferView.target == 0) {
			// The bufferView with target == 0 in our model refers to
			// the skinning weights, for 25 joints, each 4x4 matrix (16 floats), totaling to 400 floats or 1600 bytes.
			// So it is considered safe to skip the warning.
			//std::cout << "WARN: bufferView.target is zero" << std::endl;
			continue;
		}

		const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];
		GLuint vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(target, vbo);
		glBufferData(target, bufferView.byteLength,
					&buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);

		vbos[i] = vbo;
	}

	// Each mesh can contain several primitives (or parts), each we need to
	// bind to an OpenGL vertex array object
	for (size_t i = 0; i < mesh.primitives.size(); ++i) {

		tinygltf::Primitive primitive = mesh.primitives[i];
		tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		for (auto &attrib : primitive.attributes) {
			tinygltf::Accessor accessor = model.accessors[attrib.second];
			int byteStride =
				accessor.ByteStride(model.bufferViews[accessor.bufferView]);
			glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

			int size = 1;
			if (accessor.type != TINYGLTF_TYPE_SCALAR) {
				size = accessor.type;
			}

			int vaa = -1;
			if (attrib.first.compare("POSITION") == 0) vaa = 0;
			if (attrib.first.compare("NORMAL") == 0) vaa = 1;
			if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
			if (attrib.first.compare("JOINTS_0") == 0) vaa = 3;
			if (attrib.first.compare("WEIGHTS_0") == 0) vaa = 4;
			if (vaa > -1) {
				glEnableVertexAttribArray(vaa);
				glVertexAttribPointer(vaa, size, accessor.componentType,
									accessor.normalized ? GL_TRUE : GL_FALSE,
									byteStride, BUFFER_OFFSET(accessor.byteOffset));
			} else {
				std::cout << "vaa missing: " << attrib.first << std::endl;
			}
		}

		// Record VAO for later use
		PrimitiveObject primitiveObject;
		primitiveObject.vao = vao;
		primitiveObject.vbos = vbos;
		primitiveObjects.push_back(primitiveObject);

		glBindVertexArray(0);
	}
}

void Robot::bindModelNodes(std::vector<PrimitiveObject> &primitiveObjects,
					tinygltf::Model &model,
					tinygltf::Node &node) {
	// Bind buffers for the current mesh at the node
	if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
		bindMesh(primitiveObjects, model, model.meshes[node.mesh]);
	}

	// Recursive into children nodes
	for (size_t i = 0; i < node.children.size(); i++) {
		assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
		bindModelNodes(primitiveObjects, model, model.nodes[node.children[i]]);
	}
}

std::vector<Robot::PrimitiveObject> Robot::bindModel(tinygltf::Model &model) {
	std::vector<PrimitiveObject> primitiveObjects;

	const tinygltf::Scene &scene = model.scenes[model.defaultScene];
	for (size_t i = 0; i < scene.nodes.size(); ++i) {
		assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
		bindModelNodes(primitiveObjects, model, model.nodes[scene.nodes[i]]);
	}

	return primitiveObjects;
}

void Robot::drawMesh(const std::vector<PrimitiveObject> &primitiveObjects,
			tinygltf::Model &model, tinygltf::Mesh &mesh) {

	for (size_t i = 0; i < mesh.primitives.size(); ++i)
	{
		GLuint vao = primitiveObjects[i].vao;
		std::map<int, GLuint> vbos = primitiveObjects[i].vbos;

		glBindVertexArray(vao);

		tinygltf::Primitive primitive = mesh.primitives[i];
		tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.at(indexAccessor.bufferView));

		glDrawElements(primitive.mode, indexAccessor.count,
					indexAccessor.componentType,
					BUFFER_OFFSET(indexAccessor.byteOffset));

		glBindVertexArray(0);
	}
}

void Robot::drawModelNodes(const std::vector<PrimitiveObject>& primitiveObjects,
					tinygltf::Model &model, tinygltf::Node &node) {
	// Draw the mesh at the node, and recursively do so for children nodes
	if ((node.mesh >= 0) && (node.mesh < model.meshes.size())) {
		drawMesh(primitiveObjects, model, model.meshes[node.mesh]);
	}
	for (size_t i = 0; i < node.children.size(); i++) {
		drawModelNodes(primitiveObjects, model, model.nodes[node.children[i]]);
	}
}
void Robot::drawModel(const std::vector<PrimitiveObject>& primitiveObjects,
			tinygltf::Model &model) {
	// Draw all nodes
	const tinygltf::Scene &scene = model.scenes[model.defaultScene];
	for (size_t i = 0; i < scene.nodes.size(); ++i) {
		drawModelNodes(primitiveObjects, model, model.nodes[scene.nodes[i]]);
	}
}

void Robot::render(glm::mat4 cameraMatrix) {
	glUseProgram(programID);

	// Set camera
	glm::mat4 mvp = cameraMatrix;
	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

	// -----------------------------------------------------------------
	// Set animation data for linear blend skinning in shader
	// -----------------------------------------------------------------
	jointMatricesID = glGetUniformLocation(programID,"u_jointMatrix");
	glUniformMatrix4fv(jointMatricesID, skinObjects[0].jointMatrices.size(), GL_FALSE, &skinObjects[0].jointMatrices[0][0][0]);
	// -----------------------------------------------------------------

	// Set light data
	glUniform3fv(lightPositionID, 1, &lightPosition[0]);
	glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);

	// Draw the GLTF model
	drawModel(primitiveObjects, model);
}

void Robot::cleanup() {
	glDeleteProgram(programID);
}