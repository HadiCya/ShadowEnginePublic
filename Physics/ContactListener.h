//
//  ContactListener.h
//  game_engine
//
//  Created by Hadi Chaaban on 4/6/24.
//

#ifndef ContactListener_h
#define ContactListener_h

#include "box2d/box2d.h"
#include "Actor.h"

class Collision {
public:
    Actor* other;
    b2Vec2 point;
    b2Vec2 relative_velocity;
    b2Vec2 normal;
};

class ContactListener : public b2ContactListener {
public:
    static const uint16 CATEGORY_COLLIDER = 0x0001;
    static const uint16 CATEGORY_TRIGGER = 0x0002;
    static const uint16 CATEGORY_NONE = 0x0004;
    
    void BeginContact(b2Contact* contact) override {
        b2Fixture* fixtureA = contact->GetFixtureA();
        b2Fixture* fixtureB = contact->GetFixtureB();
        Actor* actorA = reinterpret_cast<Actor*>(fixtureA->GetUserData().pointer);
        Actor* actorB = reinterpret_cast<Actor*>(fixtureB->GetUserData().pointer);
        
        if (fixtureA->GetFilterData().categoryBits == fixtureB->GetFilterData().categoryBits && fixtureA->GetFilterData().categoryBits != CATEGORY_NONE) {
            Collision collision;
            collision.other = actorB;
            
            b2WorldManifold world_manifold;
            contact->GetWorldManifold(&world_manifold);
            collision.point = *world_manifold.points;
            collision.relative_velocity = fixtureA->GetBody()->GetLinearVelocity() - fixtureB->GetBody()->GetLinearVelocity();
            collision.normal = world_manifold.normal;
            
            if (!fixtureA->IsSensor() && !fixtureB->IsSensor()) { //If Collider
                std::vector<std::shared_ptr<Component>> actorA_Rigidbodies = actorA->getMasterList();
                for (std::shared_ptr<Component> component : actorA_Rigidbodies) { //TODO: Fix Really bad code
                    if (component->onCollisionEnter.has_value())
                        component->onCollisionEnter.value()(component->table, collision);
                }
                
                collision.other = actorA;
                std::vector<std::shared_ptr<Component>> actorB_Rigidbodies = actorB->getMasterList();
                for (std::shared_ptr<Component> component : actorB_Rigidbodies) {
                    if (component->onCollisionEnter.has_value())
                        component->onCollisionEnter.value()(component->table, collision);
                }
            } else { //If Trigger
                collision.point = b2Vec2(-999.0f,-999.0f);
                collision.normal = b2Vec2(-999.0f,-999.0f);
                std::vector<std::shared_ptr<Component>> actorA_Rigidbodies = actorA->getMasterList();
                for (std::shared_ptr<Component> component : actorA_Rigidbodies) { //TODO: Fix Really bad code
                    if (component->onTriggerEnter.has_value())
                        component->onTriggerEnter.value()(component->table, collision);
                }
                
                collision.other = actorA;
                std::vector<std::shared_ptr<Component>> actorB_Rigidbodies = actorB->getMasterList();
                for (std::shared_ptr<Component> component : actorB_Rigidbodies) {
                    if (component->onTriggerEnter.has_value())
                        component->onTriggerEnter.value()(component->table, collision);
                }
            }
        }
    };
    void EndContact(b2Contact* contact) override {
        b2Fixture* fixtureA = contact->GetFixtureA();
        b2Fixture* fixtureB = contact->GetFixtureB();
        Actor* actorA = static_cast<Actor*>(reinterpret_cast<void*>(fixtureA->GetUserData().pointer));
        Actor* actorB = static_cast<Actor*>(reinterpret_cast<void*>(fixtureB->GetUserData().pointer));
        
        if (fixtureA->GetFilterData().categoryBits == fixtureB->GetFilterData().categoryBits && fixtureA->GetFilterData().categoryBits != CATEGORY_NONE) {
            Collision collision;
            collision.other = actorB;
            
            b2WorldManifold world_manifold;
            contact->GetWorldManifold(&world_manifold);
            collision.point = b2Vec2(-999.0f,-999.0f);
            collision.relative_velocity = fixtureA->GetBody()->GetLinearVelocity() - fixtureB->GetBody()->GetLinearVelocity();
            collision.normal = b2Vec2(-999.0f,-999.0f);
            
            if (!fixtureA->IsSensor() && !fixtureB->IsSensor()) { //If Collider
                std::vector<std::shared_ptr<Component>> actorA_Rigidbodies = actorA->getMasterList();
                for (std::shared_ptr<Component> component : actorA_Rigidbodies) { //TODO: Fix Really bad code
                    if (component->onCollisionExit.has_value())
                        component->onCollisionExit.value()(component->table, collision);
                }
                
                collision.other = actorA;
                std::vector<std::shared_ptr<Component>> actorB_Rigidbodies = actorB->getMasterList();
                for (std::shared_ptr<Component> component : actorB_Rigidbodies) {
                    if (component->onCollisionExit.has_value())
                        component->onCollisionExit.value()(component->table, collision);
                }
            } else { //If Trigger
                std::vector<std::shared_ptr<Component>> actorA_Rigidbodies = actorA->getMasterList();
                for (std::shared_ptr<Component> component : actorA_Rigidbodies) { //TODO: Fix Really bad code
                    if (component->onTriggerExit.has_value())
                        component->onTriggerExit.value()(component->table, collision);
                }
                
                collision.other = actorA;
                std::vector<std::shared_ptr<Component>> actorB_Rigidbodies = actorB->getMasterList();
                for (std::shared_ptr<Component> component : actorB_Rigidbodies) {
                    if (component->onTriggerExit.has_value())
                        component->onTriggerExit.value()(component->table, collision);
                }
            }
        }
    };
};

#endif /* ContactListener_h */
