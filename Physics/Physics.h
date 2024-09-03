//
//  Physics.h
//  game_engine
//
//  Created by Hadi Chaaban on 4/5/24.
//

#ifndef Physics_h
#define Physics_h

#include "glm/glm.hpp"
#include "RaycastCallback.h"

class Physics {
public:
    Physics() {
        gravity = b2Vec2(0.0f, 9.8f);
        physics_world = new b2World(gravity);
        contact_listener = new ContactListener();
        physics_world->SetContactListener(contact_listener);
    };
    
    void PhysicsStep() {
        physics_world->Step(1.0f / 60.0f, 8, 3);
    };
    
    static b2Body* CreateBody(b2BodyDef* bodyDef) {
        return physics_world->CreateBody(bodyDef);
    }
    
    static luabridge::LuaRef Raycast(b2Vec2 pos, b2Vec2 dir, float dist) {
        RaycastCallback* raycast_callback = new RaycastCallback();
        b2Vec2 point1 = b2Vec2(pos.x, pos.y);
        dir.Normalize();
        b2Vec2 point2 = point1 + dist * dir;
        physics_world->RayCast(raycast_callback, point1, point2);
        
        std::vector<HitResult>& results = raycast_callback->foundResults;

        if (results.empty()) {
            return luabridge::LuaRef(Component::lua_state);
        }

        auto closestHitIter = std::min_element(results.begin(), results.end(),
                                               [&point1](const HitResult& a, const HitResult& b) {
                                                   return (a.point - point1).LengthSquared() < (b.point - point1).LengthSquared();
                                               });

        if (closestHitIter != results.end()) {
            return luabridge::LuaRef(Component::lua_state, *closestHitIter);
        } else {
            return luabridge::LuaRef(Component::lua_state);
        }
    }
    
    static luabridge::LuaRef RaycastAll(b2Vec2 pos, b2Vec2 dir, float dist) {
        RaycastCallback* raycast_callback = new RaycastCallback();
        b2Vec2 point1 = b2Vec2(pos.x, pos.y);
        dir.Normalize();
        b2Vec2 point2 = point1 + dist * dir;
        physics_world->RayCast(raycast_callback, point1, point2);
        
        std::sort(raycast_callback->foundResults.begin(), raycast_callback->foundResults.end(),
                  [&point1](const HitResult& a, const HitResult& b) {
                      return (a.point - point1).LengthSquared() < (b.point - point1).LengthSquared();
                  });
        
        luabridge::LuaRef table = luabridge::newTable(Component::lua_state);
        
        int index = 1;
        for (const HitResult& hitResult : raycast_callback->foundResults) {
            table[index++] = hitResult;
        }
        
        return table;
    }

    
    b2Vec2 gravity;
    ContactListener* contact_listener;
    static inline b2World* physics_world = nullptr;
};


#endif /* Physics_h */
