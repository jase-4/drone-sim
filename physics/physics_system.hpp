#ifndef PHSYICS_SYSTEM
#define PHSYICS_SYSTEM
#include <btBulletDynamicsCommon.h>
#include "physics_component.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "entity/entity_manager.hpp"


class PhysicsSystem {
    public:
        
        btDiscreteDynamicsWorld* world = nullptr;
        btBroadphaseInterface* broadphase = nullptr;
        btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
        btCollisionDispatcher* dispatcher = nullptr;
        btSequentialImpulseConstraintSolver* solver = nullptr;

        EntityManager& entity_manager = EntityManager::getInstance();
    
        PhysicsSystem() = default;
        ~PhysicsSystem() { shutdown(); }
    
        void init();   
        void update(float deltaTime); 
        void sync_physics_to_render();
        void shutdown(); 
    
        void addRigidBody(btRigidBody* body);
    };


    

#endif