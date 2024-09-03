//
//  Actor.h
//  game_engine
//
//  Created by Hadi Chaaban on 1/31/24.
//
#ifndef Actor_h
#define Actor_h

#include <stdio.h>
#include "glm/glm.hpp"
#include <string>
#include <cmath>
#include "SDL2_image/SDL_image.h"
#include <iostream>
#include <optional>
#include <vector>
#include "Component.h"

class Actor
{
public:
    std::string name = "";
    int order = 0;
    lua_State* lua_state;
    std::unordered_map<std::string, std::shared_ptr<Component>> components = std::unordered_map<std::string, std::shared_ptr<Component>>();
    std::unordered_map<std::string, std::shared_ptr<Component>> component_of_type = std::unordered_map<std::string, std::shared_ptr<Component>>();
    std::vector<std::shared_ptr<Component>> pending_components;
    std::vector<luabridge::LuaRef> pending_components_deletion;
    std::vector<std::shared_ptr<Component>> components_sorted_by_name, on_start_components, on_update_components, on_late_update_components;
    
    bool dont_destroy = false;
    static inline int global_add_component_counter = 0;
    static inline int global_actor_order = 0;
    
    Actor() {}
    
    void setName(const std::string& newName) {
        name = newName;
    }
    
    void addPendingComponents() {
        for (std::shared_ptr<Component> component : pending_components) {
            components[component->getName()] = component;
            doLifeCycleList(component); //PENDING_REFACTOR_CHECK
        }
        pending_components.clear();
    }
    
    void doLifeCycleList(std::shared_ptr<Component> component) {
        if (component->onStart.has_value())
            on_start_components.push_back(component);
        if (component->onUpdate.has_value())
            on_update_components.push_back(component);
        if (component->onLateUpdate.has_value())
            on_late_update_components.push_back(component);
    }
    
    void addComponent(std::shared_ptr<Component> component, Actor* actor) {
        component->table["actor"] = actor;
        components[component->getName()] = component;
    }
    
    luabridge::LuaRef AddComponentByType(std::string type) {
        Component unwrapped_component = Component("r" + std::to_string(global_add_component_counter), type);
        global_add_component_counter++;
        std::shared_ptr<Component> component = std::make_shared<Component>(unwrapped_component);
        component->table["actor"] = this;
        pending_components.push_back(component);
        return component->table;
    }
    
    void RemoveComponent(luabridge::LuaRef component_ref) {
        component_ref["enabled"] = false;
        pending_components_deletion.push_back(component_ref);
    }
    
    void sort_component_list() {
         //PENDING_REFACTOR_CHECK REDUNDANT?
        on_start_components.clear();
        on_update_components.clear();
        on_late_update_components.clear();
        for (const auto & [ key, component ] : components) {
            doLifeCycleList(component);
        }
        std::sort(on_start_components.begin(), on_start_components.end(), [](std::shared_ptr<Component> a, std::shared_ptr<Component> b) {
            return a->getName() < b->getName();
        });
        std::sort(on_update_components.begin(), on_update_components.end(), [](std::shared_ptr<Component> a, std::shared_ptr<Component> b) {
            return a->getName() < b->getName();
        });
        std::sort(on_late_update_components.begin(), on_late_update_components.end(), [](std::shared_ptr<Component> a, std::shared_ptr<Component> b) {
            return a->getName() < b->getName();
        });
    }
    
    std::string GetName() const {
        return name;
    }
    
    int GetID() const {
        return order;
    }
    
    void sortMasterList() {
        components_sorted_by_name.clear();
        for (const auto & [ key, component ] : components) {
            components_sorted_by_name.push_back(component);
        }
        std::sort(components_sorted_by_name.begin(), components_sorted_by_name.end(), [](std::shared_ptr<Component> a, std::shared_ptr<Component> b) {
            return a->getName() < b->getName();
        });
    }
    
    std::vector<std::shared_ptr<Component>> getMasterList() {
        sortMasterList();
        return components_sorted_by_name;
    }
    
    luabridge::LuaRef GetComponentByKey(std::string key) {
        if (components.find(key) != components.end() && components[key]->table["enabled"]) { //This and other getcomponent functions disallow getting and re enabling component
            return components[key]->table;
        }
        return luabridge::LuaRef(lua_state);
    }
    
    luabridge::LuaRef GetComponent(std::string type_name) { //TODO: Make O(1) (consider hashmap[string (type), vector<component>] approach
        sortMasterList();
        
        for (std::shared_ptr<Component> component : components_sorted_by_name ) {
            if (component->getType() == type_name && component->table["enabled"]) {
                return component->table;
            }
        }
        return luabridge::LuaRef(lua_state);
    }
    
    luabridge::LuaRef GetComponents(std::string type_name) {
        sortMasterList();
        
        luabridge::LuaRef table = luabridge::newTable(lua_state);
        
        int index = 1;
        for (std::shared_ptr<Component> component : components_sorted_by_name) { //PENDING_REFACTOR_CHECK
            if (component->getType() == type_name && component->table["enabled"]) {
                table[index++] = component->table;
            }
        }
        
        return table;
    }
    
    bool operator<(const Actor& other) const {
        return order < other.order;
    }
};

#endif

