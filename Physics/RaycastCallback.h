//
//  RaycastCallback.h
//  game_engine
//
//  Created by Hadi Chaaban on 4/6/24.
//

#ifndef RaycastCallback_h
#define RaycastCallback_h

#include "ContactListener.h"

class HitResult {
public:
    Actor* actor;
    b2Vec2 point;
    b2Vec2 normal;
    bool is_trigger;
};

class RaycastCallback : public b2RayCastCallback {
public:
    std::vector<HitResult> foundResults;
    
    float ReportFixture(b2Fixture* fixture, const b2Vec2 &point, const b2Vec2 &normal, float fraction) override {
        if (reinterpret_cast<Actor*>(fixture->GetUserData().pointer) == nullptr)
            return -1;
        if (fixture->GetFilterData().categoryBits == ContactListener::CATEGORY_NONE)
            return 1;
        
        HitResult hitResult;
        hitResult.actor = reinterpret_cast<Actor*>(fixture->GetUserData().pointer);
        hitResult.point = point;
        hitResult.normal = normal;
        hitResult.is_trigger = fixture->GetFilterData().categoryBits == ContactListener::CATEGORY_TRIGGER;
        foundResults.push_back(hitResult);
        
        return 1;
    };
};

#endif /* RaycastCallback_h */
