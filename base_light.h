#ifndef BASE_LIGHT_H
#define BASE_LIGHT_H


#include "light.h"
#include "shader_manager.h"





class BaseLight : public Light {
    public:
        glm::vec3 light_pos = glm::vec3(0.0f, 10.0f, 0.0f);
        glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);
        glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f);
        glm::vec3 diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
        glm::vec3 specular = glm::vec3(0.5f, 0.5f, 0.5f);

        // unsigned int shadow_width = 1024;
        // unsigned int shadow_height = 1024;
        std::unique_ptr<DepthMap> depth_map;
        std::shared_ptr<Shader> depth_shader; 


        float near_plane = 1.0f;
        float far_plane = 25.0f;
    
        // Implementing the setLightUniforms method from the Light interface

        // void set_light_uniforms(Shader& shader) const override {
        //     // Base implementation
        // }
       
        // std::vector<glm::mat4> gen_cubemap_transform() const override {
        //     std::vector<glm::mat4> shadow_transforms;
        //     shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        //     shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        //     shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        //     shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        //     shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        //     shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        //     return shadow_transforms;
        // }

        

        // void init_depth_map(){
        //     depth_map->init(shadow_width,shadow_height);
        // }

        


        glm::vec3 get_light_pos() const override {
            return light_pos;
        }

        // unsigned int get_shadow_width() const override {
        //     return shadow_width;
        // }

        // unsigned int  get_shadow_height() const override {
        //     return shadow_height;
        // }
         float get_far_plane() const override{
                return far_plane;
         }

         //this base class hhas to intialize this the dpeth_map
        GLuint get_depth_map_FBO() const override {
            return depth_map->get_fbo();
        }

        unsigned int get_depth_map_texture() const override {
            return depth_map->get_texture();
        }
        
            virtual ~BaseLight() = default;
    
    protected:
        glm::mat4 shadow_projection; // Assume this will be set based on light type
};

#endif