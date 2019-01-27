#ifndef __MODEL_H__
#define __MODEL_H__ 1

#include <string>
#include "mesh.h"
#include "assimp/material.h"

class aiNode;
struct aiScene;
class aiMesh;
class aiMaterial;

class Model {

public:
	std::vector<Texture> textures_loaded_;	//stores all the textures loaded
	std::vector<Mesh> meshes_;
	std::string directory_;
	bool gammaCorrection_;

	// Consturctor, expects a filepath to a 3D model
	Model(std::string const &path, bool gamma = false);

	// Draws the model, and thus all its meshes
	void Draw(const Shader& shader) const;

private:
	// Loads a model with supported ASSIMP extensions from file and stores resulting meshes
	void loadModel(std::string const path);

	// Processes a node in a recursive fashion. Processes eachi individual mesh
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);

	// Checks all material textures of a given type and loads the textures
	// The required info is returned as a Texture struct
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif