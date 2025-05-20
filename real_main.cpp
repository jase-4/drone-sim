#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>




#include "globals.h"



#include "entity/entity_manager.h"

#include "renderer/model_loading_system.h"

#include "window.h"
#include "renderer.h"

#include"renderer/basic_shapes.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//#include <btBulletDynamicsCommon.h>

#include "physics/physics_system.h"
#include "drone/drone.h"
// settings

// GLOBALS

struct RenderableComponent;
//class Renderer;
// camera
Window window;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

Entity createStaticPlane(EntityManager& manager, const glm::vec3& position, const glm::vec3& normal, float size) {
    Entity plane;
    manager.addEntity(plane);

    // 1. Transform
    auto transform = std::make_shared<TransformComponent>();
    transform->position = position;
    manager.addComponent(plane.getID(), transform);

    // 2. Rendering
    Plane planeData;
    planeData.normal = normal;
    planeData.position = position;
    planeData.size = size;

    auto renderable = std::make_shared<RenderableComponent>(generatePlane(planeData));
    GLuint colorTexture = createColorTexture(glm::vec3(0.8f, 0.8f, 0.8f), 512, 512); // Grey
    TextureComponent texComp;
    texComp.id = colorTexture;
    texComp.type = "texture_diffuse";

    renderable->textures.push_back(texComp);
    renderable->meshes[0].textures.push_back(texComp);

    manager.addComponent(plane.getID(), renderable);

    // 3. Physics
    auto physics = std::make_shared<PhysicsComponent>();

    btCollisionShape* shape = new btStaticPlaneShape(
        btVector3(normal.x, normal.y, normal.z),
        0.0f // Plane constant (can adjust if needed)
    );

    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(
        btQuaternion(0, 0, 0, 1),
        btVector3(position.x, position.y, position.z)
    ));

    btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, motionState, shape, btVector3(0, 0, 0));
    physics->body = new btRigidBody(rbInfo);

    manager.addComponent(plane.getID(), physics);

    return plane;
}


Entity createPhysicsSphere(EntityManager& manager, const glm::vec3& position, float radius, float mass, glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f)) {
    Entity sphere;
    manager.addEntity(sphere);

    // 1. Transform
    auto transform = std::make_shared<TransformComponent>();
    transform->position = position;
    manager.addComponent(sphere.getID(), transform);

    // 2. Rendering
    Sphere sphereData;
    sphereData.position = position;
    sphereData.radius = radius;
    sphereData.segments = 64;

    auto renderable = std::make_shared<RenderableComponent>(generateSphere(sphereData));
    GLuint colorTexture = createColorTexture(color, 512, 512); // Red
    TextureComponent texComp;
    texComp.id = colorTexture;
    texComp.type = "texture_diffuse";

    renderable->textures.push_back(texComp);
    renderable->meshes[0].textures.push_back(texComp);

    manager.addComponent(sphere.getID(), renderable);

    // 3. Physics (only if you have a PhysicsComponent setup)
    auto physics = std::make_shared<PhysicsComponent>();

    btCollisionShape* shape = new btSphereShape(radius);
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(
        btQuaternion(0, 0, 0, 1),
        btVector3(position.x, position.y, position.z)
    ));

    btVector3 inertia(0, 0, 0);
    if (mass != 0.0f) {
        shape->calculateLocalInertia(mass, inertia);
    }

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, inertia);
    physics->body = new btRigidBody(rbInfo);

    manager.addComponent(sphere.getID(), physics);

    return sphere;
}


Entity createPhysicsCube(EntityManager& manager, const glm::vec3& position, const glm::vec3& halfExtents, float mass, glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f)) {
    Entity cube;
    manager.addEntity(cube);

    // 1. Transform
    auto transform = std::make_shared<TransformComponent>();
    transform->position = position;
    manager.addComponent(cube.getID(), transform);

    // 2. Rendering
    auto renderable = std::make_shared<RenderableComponent>(generateCube(halfExtents)); // <- half extents version
    GLuint colorTexture = createColorTexture(color, 512, 512); // Green by default
    TextureComponent texComp;
    texComp.id = colorTexture;
    texComp.type = "texture_diffuse";

    renderable->textures.push_back(texComp);
    renderable->meshes[0].textures.push_back(texComp);
    manager.addComponent(cube.getID(), renderable);

    // 3. Physics
    auto physics = std::make_shared<PhysicsComponent>();

    btBoxShape* shape = new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z)); // <--- Bullet expects half extents too

    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(
        btQuaternion(0, 0, 0, 1),
        btVector3(position.x, position.y, position.z)
    ));

    btVector3 inertia(0, 0, 0);
    if (mass != 0.0f) {
        shape->calculateLocalInertia(mass, inertia);
    }

    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, inertia);
    physics->body = new btRigidBody(rbInfo);

    manager.addComponent(cube.getID(), physics);

    return cube;
}

void runIOContext(boost::asio::io_context& io) {
    io.run();  // Run the io_context in a separate thread
}



#include "drone/asyc_udp.h"
#include "drone/drone_manager.h"
#include "..\drone\sensors\imu.h"
#include "..\drone\sensors\gps.h"



int main()
{
    PhysicsSystem physics;
    physics.init();
    // btVector3 gravity(0, -9.8, 0);
    // std::cout << "Gravity: " << gravity.getY() << " m/s^2" << std::endl;
  
    window.init();

    Renderer renderer;
    renderer.init();

    // Communicator& comm = Communicator::getInstance();
    // comm.start();
    
    Drone drone(glm::vec3(0, 0, 0), glm::vec3(0.5f, 0.1f, 0.5f), 12.0f);
    DroneManager dm(drone);
    dm.addSensor<IMUSensor>(std::chrono::milliseconds(20));
    dm.addSensor<GPSSensor>(std::chrono::milliseconds(20));
   // drone.addSensor<GPSSensor>( std::chrono::milliseconds(100));
    dm.startSensors();
//     int oop = 9;
//     boost::asio::io_context io;
//     MQTT mqtt(io);  // Your MQTT wrapper

//     // Create the IMU sensor
//    auto imu = std::make_unique<IMUSensor>(io, mqtt, std::chrono::milliseconds(20));
//     auto gps = std::make_unique<GPSSensor>(io, mqtt, std::chrono::milliseconds(100));

//    imu->start();  // Starts the sensor loop
//     gps->start();  // Starts the sensor loop

    dm.mqtt.subscribe("udp/inbound", [](const std::string& msg) {
       std::cout << "Received: " << msg << std::endl;
        
       // std::cout << oop;
    });


    //AsyncUDP udp(io, 12345, mqtt);
    // io.run();  // Runs the event loop (required for Boost.Asio)ojjj
 
    // std::thread ioThread(runIOContext, std::ref(io));
   

    EntityManager& manager = EntityManager::getInstance();

   

    Entity groundPlane = createStaticPlane(manager, glm::vec3(0.0f, -0.4f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 50.0f);
    physics.addRigidBody(manager.getComponent<PhysicsComponent>(groundPlane.getID())->body);

    //Drone drone(glm::vec3(0, 0, 0), glm::vec3(0.5f, 0.1f, 0.5f), 15.0f);
  
    physics.addRigidBody(manager.getComponent<PhysicsComponent>(drone.drone_entity.getID())->body);
    

    // In game loop:
    drone.setRotorSpeed(0,3.01f); // Front right
    drone.setRotorSpeed(1, 3.01f); // Front left
    drone.setRotorSpeed(2, 3.0f); // Back left
    drone.setRotorSpeed(3, 3.0f);// Back right

   

    stbi_set_flip_vertically_on_load(true);

  
    glEnable(GL_DEPTH_TEST);
   // glEnable(GL_CULL_FACE);

  
    // -----------

    std::cout << "heyyyyyyyyyyyyyyyy\n" ;

    //float dt = calculateDeltaTime();
       
    while (!window.should_close())
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        window.process_input();

        dm.update(deltaTime);
       // std::cout << drone.getPosition().y << "\n";

        physics.update(deltaTime);
        
        renderer.render();


        window.swap_buffers();
        window.poll_events();
    
    }
    // io.stop();
    // ioThread.join();



   
    window.terminate();
    return 0;
}

