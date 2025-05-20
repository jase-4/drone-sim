#ifndef ENTITY_H
#define  ENTITY_H


class Entity {
    public:
        using ID = unsigned int;
        static ID currentID;
    
        Entity() : id(currentID++) {}
    
        ID getID() const { return id; }
    
    private:
        ID id;
    };
    
 Entity::ID Entity::currentID = 0; // Initialize the unique ID generator

 #endif