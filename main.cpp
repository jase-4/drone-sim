#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



#include "entity/entity_manager.hpp"


#include "core/window.hpp"
#include "core/globals.hpp"

#include "renderer/render_components.hpp"

#include "renderer/renderer.hpp"

#include"renderer/basic_shapes.h"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#include "physics/physics_system.hpp"
#include "physics/physics_component.hpp"

#include "drone/drone.hpp"

#include "drone/metrics.hpp"
// #include "drone/asyc_udp.hpp"
#include "drone/drone_manager.hpp"
#include "..\drone\sensors\imu.h"
#include "..\drone\sensors\gps.h"
#include "..\drone\sensors\barometer.h"
#include "..\drone\sensors\magnetometer.h"
#include "..\drone\sensors\ranger_finder.h"


void runIOContext(boost::asio::io_context& io);
Entity createPhysicsCube(EntityManager& manager, const glm::vec3& position, const glm::vec3& halfExtents, float mass, glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f));
Entity createStaticPlane(EntityManager& manager, const glm::vec3& position, const glm::vec3& normal, float size);
Entity createPhysicsSphere(EntityManager& manager, const glm::vec3& position, float radius, float mass, glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f));
void log_throughput() {
    uint64_t last_count = 0;
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        uint64_t current_count = message_count.load();
        std::cout << "Throughput: " << (current_count - last_count) << " messages/sec\n";
        last_count = current_count;
    }
}

int main()
{
    PhysicsSystem physics;
    physics.init();
 
  
    window.init();

    Renderer renderer;
    renderer.init();

    std::thread(log_throughput).detach();

    Drone drone(glm::vec3(0, 1.5, 0), glm::vec3(0.5f, 0.25f, 0.5f), 1.5f);
    DroneManager dm(drone);
    dm.addSensor<IMUSensor>(std::chrono::milliseconds(20));
    dm.addSensor<GPSSensor>(std::chrono::milliseconds(20));
    dm.addSensor<BarometerSensor>(std::chrono::milliseconds(50));
    dm.addSensor<MagnetometerSensor>(std::chrono::milliseconds(50));
    dm.addSensor<RangefinderSensor>(std::chrono::milliseconds(50));

    dm.startSensors();
//     boost::asio::io_context io;
//     MQTT mqtt(io);  // Your MQTT wrapper

//     // Create the IMU sensor
//    auto imu = std::make_unique<IMUSensor>(io, mqtt, std::chrono::milliseconds(20));
//     auto gps = std::make_unique<GPSSensor>(io, mqtt, std::chrono::milliseconds(100));

//    imu->start();  // Starts the sensor loop
//     gps->start();  // Starts the sensor loop

    // dm.mqtt.subscribe("udp/inbound", [](const std::string& msg) {
    //    std::cout << "Received: " << msg << std::endl;
        
    //    // std::cout << oop;
    // });

    dm.subscribe();


    //AsyncUDP udp(io, 12345, mqtt);
    // io.run();  // Runs the event loop (required for Boost.Asio)ojjj
 
    // std::thread ioThread(runIOContext, std::ref(io));
   

    EntityManager& manager = EntityManager::getInstance();
    

   

    Entity groundPlane = createStaticPlane(manager, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 50.0f);
   

    physics.addRigidBody(manager.getComponent<PhysicsComponent>(groundPlane.getID())->body);

    //Drone drone(glm::vec3(0, 0, 0), glm::vec3(0.5f, 0.1f, 0.5f), 15.0f);
  
    physics.addRigidBody(manager.getComponent<PhysicsComponent>(drone.drone_entity.getID())->body);
    

    // In game loop:
    //  drone.setRotorSpeed(0,4.0); // Front right
    //  drone.setRotorSpeed(1, 4.0); // Front left
    //  drone.setRotorSpeed(2, 4.0); // Back left
    //  drone.setRotorSpeed(3, 4.0);// Back right

   

    stbi_set_flip_vertically_on_load(true);

  
    glEnable(GL_DEPTH_TEST);




    const float fixedDeltaTime = 0.02f;
    float accumulator = 0.0f;
    float currentTime = static_cast<float>(glfwGetTime());
    
    
    while (!window.should_close())
    {
        float newTime = static_cast<float>(glfwGetTime());
        float frameTime = newTime - currentTime;
        currentTime = newTime;
        accumulator += frameTime;

        window.process_input();

        while (accumulator >= fixedDeltaTime)
        {
            dm.update(fixedDeltaTime);       
            physics.update(fixedDeltaTime);  
            accumulator -= fixedDeltaTime;
        }

     
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer.render();

        window.swap_buffers();  
        window.poll_events();
    }
       
    

   
    window.terminate();
    return 0;
}

void runIOContext(boost::asio::io_context& io) {
    io.run();  
}

Entity createStaticPlane(EntityManager& manager, const glm::vec3& position, const glm::vec3& normal, float size) {
    Entity plane;
    manager.addEntity(plane);

   
    auto transform = std::make_shared<TransformComponent>();
    transform->position = position;
    manager.addComponent(plane.getID(), transform);

    Plane planeData;
    planeData.normal = normal;
    planeData.position = position;
    planeData.size = size;

    auto renderable = std::make_shared<RenderableComponent>(generatePlane(planeData));
    GLuint colorTexture = createColorTexture(glm::vec3(0.8f, 0.8f, 0.8f), 512, 512); 
    TextureComponent texComp;
    texComp.id = colorTexture;
    texComp.type = "texture_diffuse";

    renderable->textures.push_back(texComp);
    renderable->meshes[0].textures.push_back(texComp);

    manager.addComponent(plane.getID(), renderable);

    auto physics = std::make_shared<PhysicsComponent>();

    btCollisionShape* shape = new btStaticPlaneShape(
        btVector3(normal.x, normal.y, normal.z),
        0.0f 
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


Entity createPhysicsSphere(EntityManager& manager, const glm::vec3& position, float radius, float mass, glm::vec3 color) {
    Entity sphere;
    manager.addEntity(sphere);

    
    auto transform = std::make_shared<TransformComponent>();
    transform->position = position;
    manager.addComponent(sphere.getID(), transform);

 
    Sphere sphereData;
    sphereData.position = position;
    sphereData.radius = radius;
    sphereData.segments = 64;

    auto renderable = std::make_shared<RenderableComponent>(generateSphere(sphereData));
    GLuint colorTexture = createColorTexture(color, 512, 512); 
    TextureComponent texComp;
    texComp.id = colorTexture;
    texComp.type = "texture_diffuse";

    renderable->textures.push_back(texComp);
    renderable->meshes[0].textures.push_back(texComp);

    manager.addComponent(sphere.getID(), renderable);

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


Entity createPhysicsCube(EntityManager& manager, const glm::vec3& position, const glm::vec3& halfExtents, float mass, glm::vec3 color) {
    Entity cube;
    manager.addEntity(cube);

   
    auto transform = std::make_shared<TransformComponent>();
    transform->position = position;
    manager.addComponent(cube.getID(), transform);

    auto renderable = std::make_shared<RenderableComponent>(generateCube(halfExtents)); 
    GLuint colorTexture = createColorTexture(color, 512, 512); 
    TextureComponent texComp;
    texComp.id = colorTexture;
    texComp.type = "texture_diffuse";

    renderable->textures.push_back(texComp);
    renderable->meshes[0].textures.push_back(texComp);
    manager.addComponent(cube.getID(), renderable);

  
    auto physics = std::make_shared<PhysicsComponent>();

    btBoxShape* shape = new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z)); 

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

