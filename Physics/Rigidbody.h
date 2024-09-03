//
//  Rigidbody.h
//  game_engine
//
//  Created by Hadi Chaaban on 4/5/24.
//

#ifndef Rigidbody_h
#define Rigidbody_h

#include "Physics.h"

struct Rigidbody {
public:
    std::string type = "Rigidbody";
    std::string key = "Rigidbody";
    std::string collider_type = "box", trigger_type = "box";
    bool enabled = true;
    Actor* actor;
    float x = 0.0f, y = 0.0f;
    float width = 1.0f, trigger_width = 1.0f, height = 1.0f, trigger_height = 1.0f, radius = 0.5f, trigger_radius = 0.5f;
    float friction = 0.3f, bounciness = 0.3f;
    std::string body_type = "dynamic";
    bool precise = true;
    float gravity_scale = 1.0f, density = 1.0f, angular_friction = 0.3f, rotation = 0.0f;
    bool has_collider = true, has_trigger = true;
    std::shared_ptr<b2FixtureDef> collider_fixture, trigger_fixture;
    std::shared_ptr<b2PolygonShape> collider_box, trigger_box;
    std::shared_ptr<b2CircleShape> collider_circle, trigger_circle;
    
    b2Body* body;
    
    void OnStart() {
        b2BodyDef body_def;
        if (body_type == "dynamic")
            body_def.type = b2_dynamicBody;
        else if (body_type == "kinematic")
            body_def.type = b2_kinematicBody;
        else if (body_type == "static")
            body_def.type = b2_staticBody;
        
        body_def.gravityScale = gravity_scale;
        body_def.position = b2Vec2(x, y);
        body_def.bullet = precise;
        body_def.angularDamping = angular_friction;
        body_def.angle = degrees_to_radians(rotation);
        
        body = Physics::CreateBody(&body_def);
        
        if (has_collider || (!has_collider && !has_trigger)) {
            collider_fixture = std::make_shared<b2FixtureDef>(); //TODO: Look into unique_ptr
            if (collider_type == "box") {
                collider_box = std::make_shared<b2PolygonShape>();
                collider_box->SetAsBox(width * 0.5f, height * 0.5f);
                collider_fixture->shape = collider_box.get();
            }
            else if (collider_type == "circle") {
                collider_circle = std::make_shared<b2CircleShape>();
                collider_circle->m_radius = radius;
                collider_fixture->shape = collider_circle.get();
            }
            collider_fixture->density = density;
            collider_fixture->restitution = bounciness;
            collider_fixture->friction = friction;
            collider_fixture->isSensor = !has_collider;
            collider_fixture->userData.pointer = reinterpret_cast<uintptr_t>(actor);
            
            b2Filter filter;
            filter.categoryBits = has_collider ? ContactListener::CATEGORY_COLLIDER : ContactListener::CATEGORY_NONE;
            collider_fixture->filter = filter;
            
            body->CreateFixture(collider_fixture.get());
        }
        if (has_trigger) {
            trigger_fixture = std::make_shared<b2FixtureDef>(); //TODO: Look into unique_ptr
            if (trigger_type == "box") {
                trigger_box = std::make_shared<b2PolygonShape>();
                trigger_box->SetAsBox(trigger_width * 0.5f, trigger_height * 0.5f);
                trigger_fixture->shape = trigger_box.get();
            }
            else if (trigger_type == "circle") {
                trigger_circle = std::make_shared<b2CircleShape>();
                trigger_circle->m_radius = trigger_radius;
                trigger_fixture->shape = trigger_circle.get();
            }
            trigger_fixture->density = density;
            trigger_fixture->restitution = bounciness;
            trigger_fixture->friction = friction;
            trigger_fixture->isSensor = true;
            trigger_fixture->userData.pointer = reinterpret_cast<uintptr_t>(actor);
            
            b2Filter filter;
            filter.categoryBits = ContactListener::CATEGORY_TRIGGER;
            trigger_fixture->filter = filter;
            
            body->CreateFixture(trigger_fixture.get());
        }
    }
    
    void OnDestroy() {
        Physics::physics_world->DestroyBody(body);
    }
    
    b2Vec2 GetPosition() {
        if (body == nullptr)
            return b2Vec2(x, y);
            
        return body->GetPosition();
    }
    
    float GetPositionX() {
        if (body == nullptr)
            return x;
        
        return body->GetPosition().x;
    }
    
    float GetPositionY() {
        if (body == nullptr)
            return y;
        
        return body->GetPosition().y;
    }
    
    float GetRotation() {
        return radians_to_degrees(body->GetAngle());
    }
    
    void AddForce(b2Vec2 force) {
        body->ApplyForceToCenter(force, true);
    }
    
    void SetVelocity(b2Vec2 vec) {
        body->SetLinearVelocity(vec);
    }
    
    void SetPosition(b2Vec2 pos) {
        if (body == nullptr) {
            x = pos.x;
            y = pos.y;
        } else {
            body->SetTransform(pos, body->GetAngle());
        }
    }
    
    void SetRotation(float degrees_clockwise) {
        body->SetTransform(body->GetPosition(), degrees_to_radians(degrees_clockwise));
    }
    
    void SetAngularVelocity(float degrees_clockwise) {
        body->SetAngularVelocity(degrees_to_radians(degrees_clockwise));
    }
    
    void SetGravityScale(float gravity) {
        body->SetGravityScale(gravity);
    }
    
    void SetUpDirection(b2Vec2 dir) {
        dir.Normalize();
        SetRotation(radians_to_degrees(glm::atan(dir.x, -dir.y)));
    }
    
    void SetRightDirection(b2Vec2 dir) {
        dir.Normalize();
        SetRotation(radians_to_degrees((glm::atan(dir.x, -dir.y) - (b2_pi / 2.0f))));
    }
    
    b2Vec2 GetVelocity() {
        return body->GetLinearVelocity();
    }
    
    float GetAngularVelocity() {
        return radians_to_degrees(body->GetAngularVelocity());
    }
    
    float GetGravityScale() {
        return body->GetGravityScale();
    }
    
    b2Vec2 GetUpDirection() {
        float angle = body->GetAngle();
        b2Vec2 result = b2Vec2(glm::sin(angle), -glm::cos(angle));
        result.Normalize();
        return result;
    }
    
    b2Vec2 GetRightDirection() {
        float angle = body->GetAngle();
        b2Vec2 result = b2Vec2(glm::cos(angle), glm::sin(angle));
        result.Normalize();
        return result;
    }
    
    float degrees_to_radians(float num_in_degrees) {
        return num_in_degrees * (b2_pi / 180.0f);
    }
    
    float radians_to_degrees(float num_in_radians) {
        return num_in_radians * (180.0f / b2_pi);
    }
    
};



#endif /* Rigidbody_h */
