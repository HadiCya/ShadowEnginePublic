//
//  Game.h
//  game_engine
//
//  Created by Hadi Chaaban on 3/28/24.
//

#ifndef Game_h
#define Game_h

#include "Actor.h"
#include "Scene.h"
#include "EngineUtils.h"
#include <algorithm>
#include <cstdlib>
#include <unordered_map>
#include <iostream>
#include <filesystem>
#include <vector>
#include <memory>
#include "Input.h"
#include "SDL2/SDL.h"
#include "rapidjson/document.h"
//#include "SDL2_ttf/SDL_ttf.h"
#include <glm/glm.hpp>
#include <thread>
#include <unordered_set>
#include <algorithm>
#include "Text.h"
#include "Renderer.h"
#include "Audio.h"
#include "Image.h"

class Game {
public:
    static Game& getInstance() {
        static Game instance;
        return instance;
    }
    Game(Game const&) = delete;
    void operator=(Game const&) = delete;
    
    void BeginGame();
    void update();
    void change_scenes();
    
    static void processFunction(std::string function_name, std::shared_ptr<Actor> actor, std::shared_ptr<Component> component) {
        if (!component->table["enabled"] && function_name != "OnDestroy" ) {
            return;
        }
        try {
            if (function_name == "OnStart") { //TODO: Clean
                component->onStart.value()(component->table);
            } else if (function_name == "OnUpdate") {
                component->onUpdate.value()(component->table);
            } else if (function_name == "OnLateUpdate") {
                component->onLateUpdate.value()(component->table);
            } else if (function_name == "OnDestroy") {
                component->onDestroy.value()(component->table);
            }
        } catch (luabridge::LuaException e) {
            std::string error_message = e.what();
            std::replace(error_message.begin(), error_message.end(), '\\', '/');
            std::cout << "\033[31m" << actor->name << " : " << error_message << "\033[0m" << std::endl;
        }
    };
    
    static void CppLog(std::string message) { std::cout << message << std::endl; };
    static void CppLogError(std::string message) { std::cerr << message << std::endl; };
    
    static Actor* FindActorByName(std::string name) {
        for (const auto& actor : getInstance().scene.actors) {
            if (actor->name == name && getInstance().ids_to_destroy.find(actor->GetID()) == getInstance().ids_to_destroy.end()) {
                return actor.get();
            }
        }
        for (const auto& actor : getInstance().scene.actors_to_add) {
            if (actor->name == name && getInstance().ids_to_destroy.find(actor->GetID()) == getInstance().ids_to_destroy.end()) {
                return actor.get();
            }
        }
        return nullptr;
    };

    static luabridge::LuaRef FindActorsByName(std::string name) {
        luabridge::LuaRef table = luabridge::newTable(getInstance().lua_state);

        int index = 1;
        for (const auto& actor : getInstance().scene.actors) {
            if (actor->name == name && getInstance().ids_to_destroy.find(actor->GetID()) == getInstance().ids_to_destroy.end()) {
                table[index++] = actor.get();
            }
        }
        for (const auto& actor : getInstance().scene.actors_to_add) {
            if (actor->name == name && getInstance().ids_to_destroy.find(actor->GetID()) == getInstance().ids_to_destroy.end()) {
                table[index++] = actor.get();
            }
        }
        return table;
    };
    
    static Actor* InstantiateActor(std::string actor_template_name) {
        Actor template_actor;
        if (getInstance().scene.templates.find(actor_template_name) != getInstance().scene.templates.end()) {
            template_actor = getInstance().scene.templates[actor_template_name];
        } else {
            std::cout << "error: template " << actor_template_name << " is missing";
            exit(0);
        }
        
        std::shared_ptr<Actor> shared_actor = std::make_shared<Actor>(Actor());
        shared_actor->lua_state = getInstance().lua_state;
        shared_actor->name = template_actor.name;
        
        for (const auto & [ key, value ] : template_actor.components) {
            shared_actor->addComponent(std::make_shared<Component>(Component(value)), shared_actor.get());
        }
        
        shared_actor->order = Actor::global_actor_order;
        Actor::global_actor_order++;
        
        //std::cout << "Actor: " << shared_actor->name << " with ID: " << shared_actor->order << " was created." << std::endl;
        shared_actor->sort_component_list();
        getInstance().scene.actors_to_add.push_back(shared_actor);
        return shared_actor.get();
    }
    
    static void DestroyActor(Actor actor) {
        actor.sortMasterList();
        for (std::shared_ptr<Component> component : actor.components_sorted_by_name) {
            if (component->onDestroy.has_value())
                processFunction("OnDestroy", std::make_shared<Actor>(actor), component);
            
            component->table["enabled"] = false;
            actor.components.erase(component->getName());
        }
        actor.components_sorted_by_name.clear();
        actor.on_start_components.clear();
        actor.on_update_components.clear();
        actor.on_late_update_components.clear();
        getInstance().ids_to_destroy.insert(actor.GetID());
        //std::cout << actor.GetName() << " is set for destruction with ID: " << actor.GetID() << std::endl;
    }
    
    static void QuitApplication() {
        exit(0);
    }
    
    static void SleepApplication(int milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
    
    static void OpenURLApplication(const std::string& url) {
        #if defined(_WIN32) || defined(_WIN64)
            std::string command = "start ";
        #elif defined(__APPLE__) || defined(__MACH__)
            std::string command = "open ";
        #elif defined(__linux__) || defined(linux) || defined(__linux)
            std::string command = "xdg-open ";
        #endif

        command += url;
        std::system(command.c_str());
    }
    
    static void SetSceneChange(std::string new_scene) {
        change_scene = new_scene;
    }
    
    static std::string GetCurrentScene() {
        return getInstance().scene.scene_name;
    }
    
    static void DontDestroyActor(Actor* actor) {
        actor->dont_destroy = true;
    }
    
    lua_State* lua_state;
    

private:
    Game() {};
    std::string game_title;
    static inline std::string change_scene = "";
    rapidjson::Document config = rapidjson::Document();
    bool done = false;
    Scene scene;
    std::unordered_set<int> ids_to_destroy;
};

#endif /* Game_h */
