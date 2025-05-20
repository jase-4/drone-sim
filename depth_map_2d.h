#ifndef DEPTH_MAP2D_H
#define  DEPTH_MAP2D_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "depth_map.h"

//class DepthMap;

//now depthMap cubemap

class DepthMap2D : public DepthMap
{
private:
   
public:
    DepthMap2D();
    ~DepthMap2D();

    void init(unsigned int shadow_width, unsigned int shadow_height) override {
        SHADOW_WIDTH = shadow_width;
        SHADOW_HEIGHT = shadow_height;
      
        glGenFramebuffers(1, &depthMapFBO);
        // create depth cubemap texture
        
        glGenTextures(1, &depth_map_2d);
        gen_cube_map();
        bind_depth_to_frame_buf_return();
    }

    unsigned int get_fbo() const override {
        return depthMapFBO;
    }
    unsigned int get_texture() const override {
        return depth_map_2d;
    }

    void gen_cube_map();
    void   bind_depth_to_frame_buf_return();
    unsigned int SHADOW_WIDTH;
    unsigned int SHADOW_HEIGHT;
    unsigned int depthMapFBO;
    unsigned int depth_map_2d;

};

DepthMap2D::DepthMap2D()

{
   
}

void DepthMap2D::gen_cube_map(){

    glBindTexture(GL_TEXTURE_2D, depth_map_2d);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // Smoother transitions
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // Smoother transitions
   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  

}

void DepthMap2D::bind_depth_to_frame_buf_return(){
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_2d, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  
}


DepthMap2D::~DepthMap2D()
{
}

#endif
