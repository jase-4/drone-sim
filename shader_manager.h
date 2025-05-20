#ifndef SHADER_MANAGER_H
#define  SHADER_MANAGER_H


#include <unordered_map>
#include <string>
#include <memory>

#include <iostream>
#include <cassert>  // For assert

#include "shaders.h"

class ShaderManager {
public:

    static ShaderManager& getInstance() {
        static ShaderManager instance;  // Instantiated once and persists
        return instance;
    }

   

    // Retrieve a shader by name
    std::shared_ptr<Shader> get_shader(const std::string& shaderName) {
        assert(shaders.find(shaderName) != shaders.end() && "Shader not found!");
        return shaders[shaderName];
    }
    void load_shader(const std::string& shaderName, const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr) {
        shaders[shaderName] = std::make_shared<Shader>(vertexPath, fragmentPath, geometryPath);
    }


private:
    ShaderManager() {
        // Initialize shaders upfront
        shaders["depth_cubemap"] = std::make_shared<Shader>("shaders/shadows_depth.vs", "shaders/shadows_depth.fs", "shaders/shadows_depth.gs");
        shaders["depth_map2d"] = std::make_shared<Shader>("shaders/depth_map2d.vs", "shaders/depth_map2d.fs");
        shaders["aabb"] = std::make_shared<Shader>("shaders/aabb.vs", "shaders/aabb.fs");

        shaders["lighting_shadow"] = std::make_shared<Shader>("shaders/lighting_shadow.vs", "shaders/temp.fs");
        shaders["light_cube"] = std::make_shared<Shader>("shaders/shader2.vert", "shaders/shader2.frag");
        shaders["depth_quad"] = std::make_shared<Shader>("shaders/debug_quad.vs", "shaders/debug_quad.fs");
        shaders["shadow_map2"] = std::make_shared<Shader>("shaders/shadow_map2.vs", "shaders/shadow_map2.fs");
        // Add more shaders as needed
    }
    ~ShaderManager() = default;
    
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;

 public:
    ShaderManager(const ShaderManager&) = delete;
    ShaderManager& operator=(const ShaderManager&) = delete;
};

#endif
