#ifndef PHSYICS_SYSTEM
#define PHSYICS_SYSTEM
#include <btBulletDynamicsCommon.h>
#include "physics_component.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


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
    
        void init();   // Initializes Bullet world
        void update(float deltaTime); // Steps the simulation
        void sync_physics_to_render();
        void shutdown(); // Cleanup
    
        void addRigidBody(btRigidBody* body); // Helper to add bodies
    };


    void PhysicsSystem::init() {
        broadphase = new btDbvtBroadphase();
        collisionConfiguration = new btDefaultCollisionConfiguration();
        dispatcher = new btCollisionDispatcher(collisionConfiguration);
        solver = new btSequentialImpulseConstraintSolver();
        world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    
        world->setGravity(btVector3(0, -9.81f, 0));
    }

    void PhysicsSystem::update(float deltaTime) {
        if (world) {
            world->stepSimulation(deltaTime, 10); // max 10 substeps
        }

        sync_physics_to_render();
    }

    void PhysicsSystem::addRigidBody(btRigidBody* body) {
        if (world && body) {
            world->addRigidBody(body);
        }
    }

    void PhysicsSystem::sync_physics_to_render() {

        auto entities = entity_manager.getEntitiesWithComponents<TransformComponent, PhysicsComponent>();
        for (auto entity : entities){
            auto transform = entity_manager.getComponent<TransformComponent>(entity);
            auto physics = entity_manager.getComponent<PhysicsComponent>(entity);
            btTransform btTrans;
            physics->body->getMotionState()->getWorldTransform(btTrans);
            btVector3 btPos = btTrans.getOrigin();
            btQuaternion btRot = btTrans.getRotation();
            transform->position = glm::vec3(btPos.getX(), btPos.getY(), btPos.getZ());
            transform->rotation = glm::quat(btRot.getW(), btRot.getX(), btRot.getY(), btRot.getZ());
        }
    }
    
    void PhysicsSystem::shutdown() {
        delete world;
        delete solver;
        delete dispatcher;
        delete collisionConfiguration;
        delete broadphase;
    }
    
    

#endif