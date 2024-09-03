//
//  Component.h
//  game_engine
//
//  Created by Hadi Chaaban on 3/27/24.
//

#ifndef Component_h
#define Component_h

#include <stdio.h>
#include <string>
#include <iostream>
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "rapidjson/document.h"
#include <filesystem>
#include "EngineUtils.h"

class Component {
public:
    Component () {};
    Component(const std::string& name, const std::string& type, rapidjson::Value::ConstObject variables);
    Component(const std::string& name, const std::string& type);
    Component(const std::string& type);
    Component(std::shared_ptr<Component> otherComponent);
        
    std::string getName() { return name; };
    std::string getType() { return type; };
    
    luabridge::LuaRef table = nullptr;
    std::optional<luabridge::LuaRef> onStart, onUpdate, onLateUpdate, onDestroy, onCollisionEnter, onCollisionExit, onTriggerEnter, onTriggerExit;
    
    std::unordered_map<std::string, Node> variables;
    
    void grabLifeCycle() {
        luabridge::LuaRef onStart = table["OnStart"];
        if (!onStart.isNil() && onStart.isFunction()) {
            this->onStart = onStart;
        }

        luabridge::LuaRef onUpdate = table["OnUpdate"];
        if (!onUpdate.isNil() && onUpdate.isFunction()) {
            this->onUpdate = onUpdate;
        }
        
        luabridge::LuaRef onLateUpdate = table["OnLateUpdate"];
        if (!onLateUpdate.isNil() && onLateUpdate.isFunction()) {
            this->onLateUpdate = onLateUpdate;
        }
        
        luabridge::LuaRef onDestroy = table["OnDestroy"];
        if (!onDestroy.isNil() && onDestroy.isFunction()) {
            this->onDestroy = onDestroy;
        }
        
        luabridge::LuaRef onCollisionEnter = table["OnCollisionEnter"];
        if (!onCollisionEnter.isNil() && onCollisionEnter.isFunction()) {
            this->onCollisionEnter = onCollisionEnter;
        }
        
        luabridge::LuaRef onCollisionExit = table["OnCollisionExit"];
        if (!onCollisionExit.isNil() && onCollisionExit.isFunction()) {
            this->onCollisionExit = onCollisionExit;
        }
        
        luabridge::LuaRef onTriggerEnter = table["OnTriggerEnter"];
        if (!onTriggerEnter.isNil() && onTriggerEnter.isFunction()) {
            this->onTriggerEnter = onTriggerEnter;
        }
        
        luabridge::LuaRef onTriggerExit = table["OnTriggerExit"];
        if (!onTriggerExit.isNil() && onTriggerExit.isFunction()) {
            this->onTriggerExit = onTriggerExit;
        }
    }
    
    static inline lua_State* lua_state;
    static inline std::unordered_map<std::string, std::shared_ptr<Component>> component_templates = std::unordered_map<std::string, std::shared_ptr<Component>>();
    static void load_components() {
        for(auto& p: std::filesystem::directory_iterator("resources/component_types")) {
            auto filename = p.path().filename().string();
            
            std::string extension = ".lua";
            if(filename.size() >= extension.size() && filename.substr(filename.size() - extension.size()) == extension) {
                filename = filename.substr(0, filename.size() - extension.size());
            }
            std::shared_ptr<Component> component = std::make_shared<Component>(filename);
            component_templates[filename] = component;
        }
    }
    
    static void component_exists(const std::string& name) {
        if (name == "Rigidbody")
            return;
        if (component_templates.find(name) == component_templates.end()) {
            std::cout << "error: failed to locate component " << name;
            exit(0);
        }
    }
private:
    std::string name = "";
    std::string type;
};

#endif /* Component_h */
