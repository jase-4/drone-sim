#ifndef ENTITY_MANAGER_H
#define  ENTITY_MANAGER_H

#include "entity.h"
#include "component.h"


#include <unordered_map>
#include <vector>
#include <memory>
#include <typeindex>
#include <type_traits>
#include <iostream>


class EntityManager {
    public:
        static EntityManager& getInstance() {
            static EntityManager instance;  // Singleton instance
            return instance;
        }
    
        // Add an entity to the system
        void addEntity(Entity entity) {
            entities.push_back(entity);
        }
    
        // Add component for a specific entity
        template <typename T>
        void addComponent(Entity::ID entityID, std::shared_ptr<T> component) {
            components[entityID][std::type_index(typeid(T))] = component;
        }
    
        // Retrieve component for a specific entity
        template <typename T>
        std::shared_ptr<T> getComponent(Entity::ID entityID) {
            auto entityComponents = components.find(entityID);
            if (entityComponents != components.end()) {
                auto componentIt = entityComponents->second.find(std::type_index(typeid(T)));
                if (componentIt != entityComponents->second.end()) {
                    return std::static_pointer_cast<T>(componentIt->second);
                }
            }
            return nullptr;
        }


        template <typename... Components>
        std::vector<Entity::ID> getEntitiesWithComponents() {
            std::vector<Entity::ID> matchingEntities;

            for (const auto& [entityID, componentMap] : components) {
                // Check if all provided component types exist in this entity's components
                bool hasAll = (... && (componentMap.find(std::type_index(typeid(Components))) != componentMap.end()));

                if (hasAll) {
                    matchingEntities.push_back(entityID);
                }
            }

            return matchingEntities;
        }



        ~EntityManager() = default;
    
        // Get all entities
        const std::vector<Entity>& getEntities() const { return entities; }
        EntityManager(const EntityManager&) = delete;
        EntityManager& operator=(const EntityManager&) = delete;
    
    private:
        EntityManager() {} // Private constructor for Singleton pattern

      
      
        std::vector<Entity> entities;
        std::unordered_map<Entity::ID, std::unordered_map<std::type_index, std::shared_ptr<Component>>> components;
    };

    #endif