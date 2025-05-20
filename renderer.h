#ifndef RENDERER_H
#define RENDERER_H




//#include "model.h"
// #include "depth_map.h"

//#include "mesh_object.h"

#include "globals.h"
#include <stb_image.h>

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>



#include <vector>
#include <memory>
#include <string>


#include "directional_light.h"
#include "spotlight.h"
#include "point_light.h"

#include "renderer/render_components.h"
#include "entity/entity_manager.h"


//#include <light.h>

//class MeshObject;

// class EntityManager;
// struct RenderableComponent;
// struct MeshComponent;
// struct TransformComponent;


unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}










unsigned int loadTexture(std::string path);

// struct Light {
//     glm::vec3 lightPos = glm::vec3(0.0f, 0.0f, 0.0f);
//     unsigned int SHADOW_WIDTH = 1024;
//     unsigned int SHADOW_HEIGHT = 1024;
//     float near_plane = 1.0f;
//     float far_plane = 25.0f;
//     glm::mat4 shadowProj; 

//     depth_map* d_map = nullptr;
// };

// unsigned int gen_depth_cube_map( unsigned int SHADOW_WIDTH ,unsigned int SHADOW_HEIGHT ){

//     unsigned int depthCubemap;
//     glGenTextures(1, &depthCubemap);
//     glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
//     for (unsigned int i = 0; i < 6; ++i){
//         glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//     }
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//     return depthCubemap;
// }

//you gen multiple 
//GLuint fbos[2];
//glGenFramebuffers(2, fbos);

// unsigned int gen_framebuffer(){
//     unsigned int depthMapFBO;
//     glGenFramebuffers(1, &depthMapFBO);
//     return depthMapFBO;
// }

// void framebuffer_bind_depth_buffer(unsigned int depthMapFBO, unsigned int  depthCubemap){
//     glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//     glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
//     glDrawBuffer(GL_NONE);
//     glReadBuffer(GL_NONE);
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);

// }

// std::vector<glm::mat4> gen_cubemap_transform(const Light &light){
//         std::vector<glm::mat4> shadow_transforms;
//         shadow_transforms.push_back(light.shadowProj * glm::lookAt(light.lightPos, light.lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
//         shadow_transforms.push_back(light.shadowProj * glm::lookAt(light.lightPos, light.lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
//         shadow_transforms.push_back(light.shadowProj * glm::lookAt(light.lightPos, light.lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
//         shadow_transforms.push_back(light.shadowProj * glm::lookAt(light.lightPos, light.lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
//         shadow_transforms.push_back(light.shadowProj * glm::lookAt(light.lightPos, light.lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
//         shadow_transforms.push_back(light.shadowProj * glm::lookAt(light.lightPos, light.lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
//         return  shadow_transforms;
// }




class Renderer
{
private:
    //std::vector<MeshObject> solid_models;
    std::vector<std::unique_ptr<Light>> lights;
   // MeshObject light_cube;

public:
    Renderer(/* args */);
    ~Renderer();
    void render();
    void init();
    void gen_shadow_map();

    // Shader simpleDepthShader;
    // Shader shader1;
    // Shader shader2;
    std::shared_ptr<Shader> light_shad_shader;
    unsigned int woodTexture ;
    unsigned int depthMapFBO ;
    unsigned int depthCubemap ;
    unsigned int default_text ;
    EntityManager& manager = EntityManager::getInstance();




    void draw_models(Shader &shader, std::vector<MeshComponent> meshes,bool draw_textures) 
    {
    for(int i = 0; i < meshes.size(); i++ ){

            if (draw_textures = true){
                for(unsigned int j = 0; j < meshes[i].textures.size(); j++)
                {
                    // std::string number;
                    //     std::string name = meshes[i].textures[j].type;
                    //     if(name == "texture_diffuse")
                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D,meshes[i].textures[j].id);
                       
                }


            }
            // // bind appropriate textures
            // unsigned int diffuseNr  = 1;
            // unsigned int specularNr = 1;
            // unsigned int normalNr   = 1;
            // unsigned int heightNr   = 1;
            // for(unsigned int i = 0; i < textures.size(); i++)
            // {
            //     //glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            //     // retrieve texture number (the N in diffuse_textureN)
            //     string number;
            //     string name = textures[i].type;
            //     if(name == "texture_diffuse")
            //         number = std::to_string(diffuseNr++);
            //     else if(name == "texture_specular")
            //         number = std::to_string(specularNr++); // transfer unsigned int to string
            //     else if(name == "texture_normal")
            //         number = std::to_string(normalNr++); // transfer unsigned int to string
            //      else if(name == "texture_height")
            //         number = std::to_string(heightNr++); // transfer unsigned int to string

                // now set the sampler to the correct texture unit
                //glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
                //GLuint uniformLocation = glGetUniformLocation(shader.ID, (name + number).c_str());
                //GLuint uniformLocation = glGetUniformLocation(shader.ID, "kyle");

                //glUniform1i(uniformLocation,i);
            // std::cout << "Uniform location for " << (name + number) << ": " << uniformLocation << std::endl;
                

            //std::cout << (name + number).c_str() << std::endl;
                // and finally bind the texture
                //glBindTexture(GL_TEXTURE_2D, textures[i].id);
            //     glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            //     // and finally bind the texture
            //     glBindTexture(GL_TEXTURE_2D, textures[i].id);
            // }

            
            // draw mesh
                
                glBindVertexArray(meshes[i].VAO);
                glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(meshes[i].indices.size()), GL_UNSIGNED_INT, 0);
                glBindVertexArray(0);

            // always good practice to set everything back to defaults once configured.
            glActiveTexture(GL_TEXTURE0);
            }
        }


   









    
    void render_objects(Shader &shader, bool set_textures){
     std::vector<Entity::ID> entity_list = manager.getEntitiesWithComponents<RenderableComponent,TransformComponent>();

        for(int i = 0; i < entity_list.size(); i++){

            
            //shader.setInt("hasTexture", solid_models[i].has_texture);
           
           auto render_component = manager.getComponent<RenderableComponent>(entity_list[i]);

            if(!render_component->should_render){
                continue;
            }

           auto transform = manager.getComponent<TransformComponent>(entity_list[i]);
            
           glm::mat4 model_matrix = transform->getModelMatrix();
           shader.setMat4("model", model_matrix);
           draw_models(shader,render_component->meshes,set_textures);

           
            // if(i == 1){
            //     glActiveTexture(GL_TEXTURE0);
            //     glBindTexture(GL_TEXTURE_2D,default_text);
            // }
            // solid_models[i].draw(shader);
        }
    }

    void gen_shadow_maps(){
        glEnable(GL_DEPTH_TEST);
        
      
        for (auto& light : lights) {
            //std::vector<glm::mat4> shadow_transforms = light->gen_cubemap_transform();
            //std::cout << "her ---------------------a " << std::endl;
            //std::cout << light->get_shadow_width() << std::endl;
            glViewport(0, 0, light->get_shadow_width(), light->get_shadow_height());
           // std::cout << "her ---------------------a " << std::endl;
            glBindFramebuffer(GL_FRAMEBUFFER, light->get_depth_map_FBO());
            glClear(GL_DEPTH_BUFFER_BIT);
                

            light->set_depth_map_shader();

            // shader.use();

            
            // for (unsigned int i = 0; i < 6; ++i){
            //     shader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadow_transforms[i]);
            // }
            // shader.setFloat("far_plane", light->get_far_plane());
            // shader.setVec3("lightPos", light->get_light_pos());

            //this probably shoul be dealt with a different way
           // std::cout << "start da render" << std::endl;
           //glCullFace(GL_FRONT);
                render_objects(*(light->get_depth_shader()),false);
                
           // std::cout << "render render overrrrrr" << std::endl;
           //glCullFace(GL_BACK);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
        //glClear(GL_DEPTH_BUFFER_BIT);
        
    }

    void set_lights_uniforms( std::shared_ptr<Shader> shader){
    
        for(auto& light : lights){
            light->set_light_uniforms(shader);
          
        }
    }

    void bind_shadow_map_textures(){
        for(auto& light : lights){
            light->bind_shadow_map_texture();
        }
    }

 


};



void Renderer::init(){

    


    std::cout << "im confused\n" << std::endl;

    //MeshObject cube("obj/cube2.obj");
    // MeshObject cube2("obj/cube2.obj");


    // light_cube = MeshObject("obj/cube2.obj");  

    // MeshObject new_plane("obj/cube2.obj");
    // new_plane.has_texture = 0;

    // cube.translate(0.0f, 0.0f, 0.0f);
    // cube2.translate(5.5f, 0.0f, 7.0f);

    // cube2.scale(1.0, 2.0f, 1.5f);



    // new_plane.translate(0.0f, -2.05f, 0.0f);
    // new_plane.scale(50.0f, 0.05f, 50.0f);

   
    // light_cube.scale(1.0f, 1.0f, 1.0f);
    // light_cube.translate(0.0, 10.0, 0.0f);

    // depthMapFBO = gen_framebuffer();
    // depthCubemap = gen_depth_cube_map(1024 ,1024 );
    // framebuffer_bind_depth_buffer(depthMapFBO,depthCubemap)

    // Light only_light;
    // only_light.lightPos = glm::vec3(0.0f, 10.0f, 0.0f);
    // only_light.shadowProj = glm::perspective(glm::radians(90.0f), (float)only_light.SHADOW_WIDTH / (float)only_light.SHADOW_HEIGHT, only_light.near_plane, only_light.far_plane);
    // depth_map new_d_map(only_light.SHADOW_WIDTH,only_light.SHADOW_HEIGHT);
    // std::cout << "init3333 is done\n" << endl;
    // only_light.d_map = new depth_map(only_light.SHADOW_WIDTH,only_light.SHADOW_HEIGHT);

    std::cout << "init3333 is done\n" << std::endl;

   // std::unique_ptr<DirLight> dir_light = std::make_unique<DirLight>(); 
    std::unique_ptr<Spotlight> dir_light = std::make_unique<Spotlight>(); 
    dir_light->init_depth_map();
    std::unique_ptr<PointLight> point = std::make_unique<PointLight>(); 
    point->init_depth_map();


    std::unique_ptr<Light> light = std::move(dir_light);
    std::unique_ptr<Light> light2 = std::move(point);
    lights.emplace_back(std::move(light));
    lights.emplace_back(std::move(light2));

    // Shader shader("shaders/point_shadows.vs", "shaders/point_shadows.fs");
    // Shader simpleDepthShader("shaders/shadows_depth.vs", "shaders/shadows_depth.fs", "shaders/shadows_depth.gs");

    // solid_models.emplace_back(cube);
    // solid_models.emplace_back(cube2);
    // solid_models.emplace_back(new_plane);


    light_shad_shader = ShaderManager::getInstance().get_shader("lighting_shadow");
   // light_shad_shader = ShaderManager::getInstance().get_shader("shadow_map2");

//    simpleDepthShader = Shader("shaders/shadows_depth.vs", "shaders/shadows_depth.fs", "shaders/shadows_depth.gs");
//      //shader = Shader("shaders/point_shadows.vs", "shaders/point_shadows.fs");
//      shader1 = Shader("shaders/lighting_shadow.vs", "shaders/lighting_shadow.fs");
//      shader2 = Shader("shaders/shader2.vert", "shaders/shader2.frag");

      woodTexture = loadTexture("obj/rock.jpg");



    light_shad_shader->use();
    light_shad_shader->setInt("material.diffuse", 0);
    light_shad_shader->setInt("material.specular", 1);
    //light_shad_shader->setInt("depthMap", 2);
    light_shad_shader->setInt("dirDepthMap", 2);
    light_shad_shader->setInt("depthCubeMap", 3);

    
    // light_shad_shader->use();
    // light_shad_shader->setInt("diffuseTexture", 0);
    // light_shad_shader->setInt("shadowMap", 1);
    //light_shad_shader->setInt("depthMap", 2);
   // light_shad_shader->setInt("dirDepthMap", 2);

    
    std::cout << "init is done\n" << std::endl;
    
    //light_cube.translate(0.0f, 0.0f, 0.0f);

    //unsigned int defaultTexture;
    glGenTextures(1, &default_text);
    glBindTexture(GL_TEXTURE_2D,default_text);

    // Create a single white pixel texture (RGBA = 1,1,1,1)
    unsigned char whitePixel[] = { 255, 255, 255, 0 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    //solid_models.emplace_back(light_cube);

}


//delta time and lastframe are global
void Renderer::render(){


    // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float time = glfwGetTime();  
    float minValue = -20.0f;         // Minimum value
    float maxValue = 20.0f;
    float minValue1 = 2.0f;         // Minimum value
    float maxValue1 = 30.0f;
    float range = maxValue - minValue; 
    float range1 = maxValue1 - minValue1; 
    float speed = 0.5f;      
    float speed1 = 1.0f;      
    float light_pos_z = sin(time * speed) * range / 2.0f + (minValue + maxValue) / 2.0f;
    float light_pos_y = sin(time * speed1) * range1 / 2.0f + (minValue1 + maxValue1) / 2.0f;
    //light_pos_z = 10.0f;
    lights[0]->set_light_pos(glm::vec3(lights[0]->get_light_pos().x,light_pos_y,light_pos_z));


    // float pos_z =  static_cast<float>(sin(glfwGetTime() * 0.5) * 7.0);

    // //light_cube.translate(0.0f,0.0f,pos_z);
    // lights[0].lightPos = light_cube.get_position();
    

    // std::vector<glm::mat4> shadow_transforms = lights[0].gen_cubemap_transform();


    // glViewport(0, 0, lights[0].SHADOW_WIDTH, lights[0].SHADOW_HEIGHT);
    // glBindFramebuffer(GL_FRAMEBUFFER, lights[0].d_map->depthMapFBO);
    // glClear(GL_DEPTH_BUFFER_BIT);
    // simpleDepthShader.use();

    // for (unsigned int i = 0; i < 6; ++i){
    //     simpleDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadow_transforms[i]);
    // }
    // simpleDepthShader.setFloat("far_plane", lights[0].far_plane);
    // simpleDepthShader.setVec3("lightPos", lights[0].lightPos);
    // render_objects(simpleDepthShader);
   
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //std::cout << "its thisssssss" << std::endl;

    gen_shadow_maps();

    //std::cout << "hmmmmmmm" << std::endl;


    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    
    //shader.setInt("reverse_normals", 0);


    // shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    // shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    // shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    // shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);


    // shader.setVec3("pointLights[0].position", glm::vec3(0.0f, 4.0f, 4.0f));
    // shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    // shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    // shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    // shader.setFloat("pointLights[0].constant", 1.0f);
    // shader.setFloat("pointLights[0].linear", 0.09f);
    // shader.setFloat("pointLights[0].quadratic", 0.032f);

   // float time = glfwGetTime();  
   


    light_shad_shader->use();
    set_lights_uniforms(light_shad_shader);

    light_shad_shader->setFloat("material.shininess", 32.0f);

    



    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();

    light_shad_shader->setMat4("projection", projection);
    light_shad_shader->setMat4("view", view);
    light_shad_shader->setVec3("viewPos", camera.Position);

    // set lighting uniforms
   // shader.setVec3("lightPos", lights[0].lightPos);
 
    //shader.setInt("shadows", true); // enable/disable shadows by pressing 'SPACE'
    //shader.setFloat("far_plane", lights[0].far_plane);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, woodTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,default_text);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,lights[0]->get_depth_map_texture());
    glActiveTexture(GL_TEXTURE3);
     glBindTexture(GL_TEXTURE_CUBE_MAP, lights[1]->get_depth_map_texture());



    //wait big problem here 3 texture one slot rn
    //bind_shadow_map_textures();
   
    
    // need to bind difd per light here
   // glBindTexture(GL_TEXTURE_CUBE_MAP, lights[0]->get_depth_map_texture());
   //light matrix one
    lights[0]->set_light_shad_shader_vars(light_shad_shader);
    render_objects(*light_shad_shader,true);

    // lights[1]->set_light_shad_shader_vars(light_shad_shader);
    // render_objects(*light_shad_shader);


    

    // shader2.use();
    // shader2.setMat4("projection", projection);
    // shader2.setMat4("view", view);
    // light_cube.draw(shader2);

        // std::shared_ptr<Shader>  depth_quad = ShaderManager::getInstance().get_shader("depth_quad");
        // depth_quad->use();
        // depth_quad->setFloat("near_plane",lights[0]->get_near_plane());
        // depth_quad->setFloat("far_plane",lights[0]->get_far_plane());
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, lights[0]->get_depth_map_texture());
      //renderQuad();







}

Renderer::Renderer(/* args */)
{ 
}

Renderer::~Renderer()
{
}

unsigned int loadTexture(std::string path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

#endif