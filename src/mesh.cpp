#include "mesh.h"
#include "shader.h"
#include <glad/glad.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<Texture> textures): 
	vertices_(vertices), indices_(indices), textures_(textures) {
		setupMesh();
}

void Mesh::setupMesh() {
	// Create buffers/arrays
	glGenVertexArrays(1, &VAO_);
	glGenBuffers(1, &VBO_);
	glGenBuffers(1, &EBO_);

	glBindVertexArray(VAO_);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_);

	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(uint32_t), &indices_[0], GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// Vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	// Vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	// Vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

	// Vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glBindVertexArray(0);
}

void Mesh::Draw(const Shader& shader) const {
	// Bind appropiate textures
	uint32_t diffuseNr = 1;
	uint32_t specularNr = 1;
	uint32_t normalNr = 1;
	uint32_t heightNr = 1;

	for (uint8_t i = 0; i < textures_.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i); // Active proper texutre unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		std::string number;
		std::string name = textures_[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++); // Transfer unsigned int to stream
		else if (name == "texture_normal")
			number = std::to_string(normalNr++); // Transfer unsigned int to stream
		else if (name == "texture_height")
			number = std::to_string(heightNr++); // Transfer unsigned int to stream

		// Set the sampler to the correct texture unit
		shader.set((name + number).c_str(), i);
		// Bind texture
		glBindTexture(GL_TEXTURE_2D, textures_[i].id);
	}

	// Draw mesh
	glBindVertexArray(VAO_);
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// Set everything back to defaults once configured
	glActiveTexture(GL_TEXTURE0);
}