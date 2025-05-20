#ifndef DEPTH_MAPCUBE_H
#define  DEPTH_MAPCUBE_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "depth_map_2d.h"



//now depthMap cubemap
//change name to depth_cube_map
class DepthMapCubeMap : public DepthMap
{
private:
   
public:
    DepthMapCubeMap();
    ~DepthMapCubeMap();

    void init(unsigned int shadow_width, unsigned int shadow_height) override {
        SHADOW_WIDTH = shadow_width;
        SHADOW_HEIGHT = shadow_height;
      
        glGenFramebuffers(1, &depthMapFBO);
        // create depth cubemap texture
        
        glGenTextures(1, &depthCubemap);
        gen_cube_map();
        bind_depth_to_frame_buf_return();
    }

    unsigned int get_fbo() const override {
        return depthMapFBO;
    }
    unsigned int get_texture() const override {
        return depthCubemap;
    }

    void gen_cube_map();
    void   bind_depth_to_frame_buf_return();
    unsigned int SHADOW_WIDTH;
    unsigned int SHADOW_HEIGHT;
    unsigned int depthMapFBO;
    unsigned int depthCubemap;

};

DepthMapCubeMap::DepthMapCubeMap()

{
   
}

void DepthMapCubeMap::gen_cube_map(){

    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i){
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

}

void DepthMapCubeMap::bind_depth_to_frame_buf_return(){
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


DepthMapCubeMap::~DepthMapCubeMap()
{
}

#endif
