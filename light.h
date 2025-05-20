#ifndef LIGHT_H
#define LIGHT_H



#include "shaders.h"
#include <glm/glm.hpp> 
#include <vector>
#include <glm/ext/matrix_transform.hpp>

#include "depth_map_2d.h"
#include "depth_map_cube_map.h"


#include <memory>

class Light
{

public:
    //virtual void set_light_uniforms() const = 0;
   // virtual std::vector<glm::mat4> gen_cubemap_transform() const = 0;

    virtual glm::vec3 get_light_pos() const = 0;
    virtual void set_light_pos(glm::vec3 new_pos) = 0;


    virtual unsigned int get_shadow_width() const = 0;
    virtual unsigned int get_shadow_height() const = 0;
    virtual GLuint get_depth_map_FBO() const = 0;
    virtual void set_light_uniforms(std::shared_ptr<Shader> shader)  = 0;
    virtual float get_far_plane() const = 0;
    virtual float get_near_plane() const = 0;

    //virtual void gen_shadow_map() const = 0;

    virtual unsigned int get_depth_map_texture() const= 0;

    virtual std::shared_ptr<Shader> get_depth_shader() const = 0;  // Pure virtual functi

    virtual void set_depth_map_shader()  = 0;
    virtual void bind_shadow_map_texture() const = 0;

    virtual void set_light_shad_shader_vars(std::shared_ptr<Shader> shader) =0;

    virtual ~Light() {}
};

#endif

