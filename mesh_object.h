#ifndef MESH_OBJECT_H
#define  MESH_OBJECT_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "shaders.h"
#include "model.h"

class MeshObject 
{
private:
   
public:
    glm::mat4 model_matrix = glm::mat4(1.0f);
   
    Model model;
    int has_texture = 1;

    MeshObject(string const &path);
    MeshObject(){};
    ~MeshObject();

    void draw(Shader &shader)  {
        //shader.use();
        shader.setMat4("model", model_matrix);
        //shader.setInt("hasTexture",has_texture);
        model.Draw(shader);
        
    }

    glm::vec3 get_position() {
        return glm::vec3(model_matrix[3]); // Extracts the 4th column as a vec3
    }

    void translate(float x, float y, float z){
        model_matrix = glm::translate(model_matrix, glm::vec3(x, y,z));
    }
    void scale(float x, float y, float z){
        model_matrix =  glm::scale(model_matrix, glm::vec3(x, y,z));
    }

    void rotate_x(glm::f32 angle){
        model_matrix =  glm::rotate(model_matrix, glm::radians(angle),glm::vec3(1.0f, 0.0f, 0.0f));
    }

    void rotate_y(glm::f32 angle){
        model_matrix = glm::rotate(model_matrix, glm::radians(angle),glm::vec3(0.0f, 1.0f, 0.0f));
    }


    void rotate_z(glm::f32 angle){
        model_matrix =  glm::rotate(model_matrix, glm::radians(angle),glm::vec3(0.0f, 0.0f, 1.0f));
    }
};

MeshObject::MeshObject(string const &path) : model(path) {
    //model = Model(path);
}

MeshObject::~MeshObject()
{
}

#endif