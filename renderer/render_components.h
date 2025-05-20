

#ifndef RENDER_COMPONENETS_H
#define RENDER_COMPONENETS_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shaders.h"

//#include "entity/component.h"

#include <string>
#include <vector>


#define MAX_BONE_INFLUENCE 4

class Component;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
	//bone indexes which will influence this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	//weights from each bone
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct TextureComponent {
    unsigned int id;
    std::string type;
    std::string path;
};




struct TransformComponent :  public Component {
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // Identity quaternion
    glm::vec3 scale = glm::vec3(1.0f); // Default scale is 1,1,1

    // Helper to get transformation matrix
    glm::mat4 getModelMatrix() const {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 rotationMat = glm::mat4_cast(rotation);
        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);
        return translation * rotationMat * scaleMat;
    }
};



struct MeshComponent {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureComponent>      textures;
  
    GLuint VAO, VBO, IBO;

    MeshComponent(const std::vector<Vertex>& verts,
        const std::vector<unsigned int>& inds,
        const std::vector<TextureComponent>& texs)
    : vertices(verts), indices(inds), textures(texs) {}
};



struct RenderableComponent :public Component {
    std::vector<MeshComponent> meshes; // Holds the meshes for the entity
    std::vector<TextureComponent> textures; // Holds the textures for the entity
    bool should_render = true;
    bool isTextured = true;
    glm::vec3 color = glm::vec3(0.8, 0.8, 0.8);
};





void printVec3(const glm::vec3& vec, const std::string& label = "") {
    std::cout << label << ": (" << vec.x << ", " << vec.y << ", " << vec.z << ")\n";
}

void printEulerFromQuat(const glm::quat& q)
{
    glm::vec3 eulerDegrees = glm::degrees(glm::eulerAngles(q));

    std::cout << "Euler angles (degrees):" << std::endl;
    std::cout << "  Pitch (X): " << eulerDegrees.x << std::endl;
    std::cout << "  Yaw   (Y): " << eulerDegrees.y << std::endl;
    std::cout << "  Roll  (Z): " << eulerDegrees.z << std::endl;
}

void printMat3(const glm::mat3& mat, const std::string& label = "")
{
    std::cout << label << ":\n";
    for (int i = 0; i < 3; ++i) {
        std::cout << "  (" << mat[i][0] << ", " << mat[i][1] << ", " << mat[i][2] << ")\n";
    }
}



#endif