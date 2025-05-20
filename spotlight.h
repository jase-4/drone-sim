#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H


#include "light.h"
#include "shader_manager.h"

//#include "globals.h"

//#include "depth_map_2d.h"

class DepthMap;
class DepthMap2D;


class Spotlight : public Light
{
private:
    /* data */
public:

    // glm::vec3 light_pos = glm::vec3(0.0f, 10.0f, 0.0f);

    // glm::vec3 direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    // glm::vec3 ambient = glm::vec3(0.05f, 0.05f, 0.05f);
    // glm::vec3 diffuse =glm::vec3(0.4f, 0.4f, 0.4f);
    // glm::vec3 specular =glm::vec3( 0.5f, 0.5f, 0.5f);

        glm::vec3 light_pos = glm::vec3(10.0f, 10.0f, 0.0);
        glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);
        float cut_off =glm::cos(glm::radians(20.5f));
        float outer_cut_off = glm::cos(glm::radians(35.0f));

        float constant = 1.0f;
        float linear = 0.02f;
        float quadratic = 0.001f;
        glm::vec3 ambient = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);

        std::unique_ptr<DepthMap> depth_map;
        //std::shared_ptr<Shader> depth_shader; 

        float near_plane =0.1f;
        float far_plane = 175.0f;

  
    //glm::mat4 shadow_projection = glm::perspective(glm::radians(90.0f), (float)shadow_width / (float)shadow_height, near_plane, far_plane);
    //float orthoSize = 30.0f; // Adjust based on scene size

    glm::vec3 targetPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

   
    unsigned int shadow_width = 1024  ;
    unsigned int shadow_height = 1024  ;

    //float near_plane = 1.0f, far_plane = 50.0f;

    glm::mat4 light_projection = glm::perspective(glm::radians(45.0f), (float)shadow_width / (float)shadow_height, near_plane, far_plane);
    
   // glm::mat4 light_projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    bool shadows = false;
    std::shared_ptr<Shader> depth_shader = ShaderManager::getInstance().get_shader("depth_map2d");


    void init_depth_map(){
      //  std::cout <<"jkhfsdafjsafsh" << endl;

        std::cout << shadow_width << std::endl;

        // std::unique_ptr<DepthMap2D> depth_map_t = std::make_unique<DepthMap2D>();
        // std::unique_ptr<DepthMap> light = std::move(depth_map_t);
        std::unique_ptr<DepthMap2D>  depth_map_2d = std::make_unique<DepthMap2D>();
        depth_map = std::move(depth_map_2d);

        // std::unique_ptr<DepthMap2D> depth_map_2d = std::make_unique<DepthMap2D>();
        // depth_map = std::move(depth_map_2d);  // Ownership transferred
        depth_map->init(shadow_width,shadow_height);
    }

    std::shared_ptr<Shader> get_depth_shader() const override{
        return depth_shader;
    }

     void set_light_pos(glm::vec3 new_pos) override {
        light_pos = new_pos;
    }

    
    unsigned int get_shadow_width() const override {
        return shadow_width;
    }

    unsigned int  get_shadow_height() const override {

        return shadow_height;
    }

    float get_far_plane() const override{
            return far_plane;
    }

    float get_near_plane() const override{
        return near_plane;
}

    //this base class hhas to intialize this the dpeth_map
    GLuint get_depth_map_FBO() const override {
        return depth_map->get_fbo();
    }

    unsigned int get_depth_map_texture() const override {
        return depth_map->get_texture();
    }

    

    
   

    // std::vector<glm::mat4> gen_cubemap_transform() const override{
    //     std::vector<glm::mat4> shadow_transforms;
    //     shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    //     shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos,light_pos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    //     shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    //     shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    //     shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    //     shadow_transforms.push_back(shadow_projection * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    //     return  shadow_transforms;
    // }

    void set_light_shad_shader_vars(std::shared_ptr<Shader> shader) override{

        glm::mat4 lightView = glm::lookAt(light_pos, targetPos, upVector);
        
        glm::mat4 lightSpaceMatrix = light_projection * lightView;
        shader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        //shader map 2
        //shader->setVec3("lightPos", light_pos);
    }

    void print_glm(glm::vec3 pos){
        std::cout << pos.x << " " << pos.y <<  " " << pos.z <<std::endl;
    }

    void set_depth_map_shader()  override {

       
        glm::mat4 view = camera.GetViewMatrix();
        // glViewport(0, 0, get_shadow_width(), get_shadow_height());
        // glBindFramebuffer(GL_FRAMEBUFFER, get_depth_map_FBO());
        // glClear(GL_DEPTH_BUFFER_BIT);

    
        glm::mat4 lightView = glm::lookAt(light_pos, targetPos, upVector);
       
        glm::mat4 lightSpaceMatrix = light_projection * lightView;

        depth_shader->use();
        depth_shader->setMat4("lightSpaceMatrix",lightSpaceMatrix);

       
        // depth_shader->setFloat("far_plane", get_far_plane());
        // depth_shader->setVec3("lightPos", get_light_pos());
        
      
       
        // depth_shader->setMat4("projection", light_projection);
        // depth_shader->setMat4("view", view);



    }


    

    glm::vec3 get_light_pos() const override {
        return light_pos;
    }

// hfasfkjghsakjlghsaghj
    void bind_shadow_map_texture() const override {
        unsigned int default_text; 
        glGenTextures(1, &default_text);
        glBindTexture(GL_TEXTURE_2D,default_text);
    
        // Create a single white pixel texture (RGBA = 1,1,1,1)
        unsigned char whitePixel[] = { 1, 1, 1, 1 };
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, default_text);
        //glBindTexture(GL_TEXTURE_2D, depth_map->get_texture());
    }
    

   void set_light_uniforms(std::shared_ptr<Shader> shader)  override {
        //shared_ptr<Shader> depth_shader = ShaderManager::getInstance().get_shader("depth_map2d");

        direction = glm::normalize(targetPos - light_pos);

        shader->setVec3("spotLight.position",light_pos);
        shader->setVec3("spotLight.direction",direction);
        shader->setFloat("spotLight.cutOff",cut_off);
        shader->setFloat("spotLight.outerCutOff",outer_cut_off);
        shader->setFloat("spotLight.constant",constant);
        shader->setFloat("spotLight.linear",linear);
        shader->setFloat("spotLight.quadratic",quadratic);
        shader->setVec3("spotLight.ambient",ambient);
        shader->setVec3("spotLight.diffuse",diffuse);
        shader->setVec3("spotLight.specular",specular);

        //shader->setVec3("lightPos", light_pos);
        //shader->setVec3("viewPos", camera.Position);
        //shader->setFloat("far_plane", far_plane);


   }

    Spotlight(/* args */);
    ~Spotlight();
};

Spotlight::Spotlight(/* args */)
{
}

Spotlight::~Spotlight()
{
}

#endif
