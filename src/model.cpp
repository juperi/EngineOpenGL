#define STB_IMAGE_IMPLEMENTATION 

#include "model.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>
#include <glad/glad.h>
#include <stb_image.h>

Model::Model(std::string const &path, bool gamma) : gammaCorrection_(gamma) {
	loadModel(path);
}

void Model::loadModel(std::string const path) {
	// Read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	// Check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}
	// Retreieve the directory path of the filepath
	directory_ = path.substr(0, path.find_last_of('/'));

	// Process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene) {
	// Process each mesh located at the current node
	for (uint32_t i = 0; i < node->mNumMeshes; i++) {
		// The node object only contains indices to index the actual object in the scene
		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes)
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes_.push_back(processMesh(mesh, scene));
	}
	// Process recursively each of the children nodes after all meshes have been processed
	for (uint32_t i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene) {
	// Data to fill
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Texture> textures;

	// Each of the mesh's vertices
	for (uint32_t i = 0; i < mesh->mNumVertices; i++) { 
		Vertex vertex; 
		glm::vec3 vector; // Placeholder vector (assimp uses its own vector class)
		
		// Positions
		vector.x = mesh->mVertices[i].x; 
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		
		// Normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		
		// Texture coordinates
		if (mesh->mTextureCoords[0]) { // Mesh contain texture coord?
			glm::vec2 vec;
			// A vertex can contain up to 8 different texture coordinates
			// Use models where a vertex can have multiple texture coordinates so we always take the first set (0)
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		
		// Tangent
		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		vertex.Tangent = vector;
		
		// Bitangent
		vector.x = mesh->mBitangents[i].x; 
		vector.y = mesh->mBitangents[i].y;
		vector.z = mesh->mBitangents[i].z;
		vertex.Bitangent = vector;
		vertices.push_back(vertex);
	}

	// Each of mesh's faces
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) { 
		aiFace face = mesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// Process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_difusseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.

	// 1. Diffuse maps
	std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");

	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	
	// 4. height maps
	std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT,	"texture_height");
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	return Mesh(vertices, indices, textures);
}

static unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamme) {
	std::string fileName = std::string(path);
	fileName = directory + '/' + fileName;
	
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int widht, height, nrComponents;
	unsigned char * data = stbi_load(fileName.c_str(), &widht, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1) {
			format = GL_RED;
		}
		else if (nrComponents == 3) {
			format = GL_RGB;
		}
		else if (nrComponents == 4) {
			format = GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, widht, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Failed to load the texture at path: " << path << std::endl;
		stbi_image_free(data);
	}
	return textureID;
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
	std::vector<Texture> textures;
	for (uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);

		// Dheck if texture was loaded before
		bool skip = false; 
		for (uint32_t j = 0; j < textures_loaded_.size(); j++) {
			if (std::strcmp(textures_loaded_[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded_[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to the next
				break;
			}
		}
		// if texture hasn't been loaded already, load it
		if (!skip) { 
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), directory_, false);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded_.push_back(texture); // Store it as a texture loaded for entire model
		}
	}
	return textures;
}

void Model::Draw(const Shader& shader) const {
	for (uint32_t i = 0; i < meshes_.size(); i++)
		meshes_[i].Draw(shader);
}