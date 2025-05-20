#ifndef DEPTH_MAP_H
#define DEPTH_MAP_H

class DepthMap
{
private:
    /* data */
public:
    virtual void init(unsigned int shadow_width, unsigned int shadow_height) = 0;
    virtual unsigned int get_fbo() const = 0;
    virtual unsigned int get_texture() const = 0;

   virtual ~DepthMap() {};
};

#endif