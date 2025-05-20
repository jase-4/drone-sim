#ifndef GLOBALS_H
#define GLOBALS_H

#include "camera.h"




class Window;

extern const unsigned int SCR_WIDTH = 800;
extern unsigned int SCR_HEIGHT = 600;

extern float lastX = SCR_WIDTH / 2.0f;
extern float lastY = SCR_HEIGHT / 2.0f;
extern bool firstMouse = true;

// timing
extern float deltaTime = 0.0f;
extern float lastFrame = 0.0f;


extern Camera camera;
extern Window window;


#endif