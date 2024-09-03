//
//  Scene.h
//  game_engine
//
//  Created by Hadi Chaaban on 1/31/24.
//

#ifndef Scene_h
#define Scene_h

#include "rapidjson/document.h"
#include "Actor.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include "SDL2/SDL.h"
#include "SDL2_image/SDL_image.h"
#include "Component.h"

class Scene {
public:
    Scene(std::string scene_name, lua_State* lua_state);
    Scene();
    void scene_exists(const std::string& name);
    void load_actors();
    void load_templates();
    void load_components();
    
    void sort_actor_list() {
        std::sort(actors_to_add.begin(), actors_to_add.end(), [](std::shared_ptr<Actor> a, std::shared_ptr<Actor> b) {
            return a->GetID() < b->GetID();
        });
        std::sort(actors.begin(), actors.end(), [](std::shared_ptr<Actor> a, std::shared_ptr<Actor> b) {
            return a->GetID() < b->GetID();
        });
    }
    
    std::string scene_name;
    std::vector<std::shared_ptr<Actor>> actors = std::vector<std::shared_ptr<Actor>>();
    std::vector<std::shared_ptr<Actor>> actors_to_add = std::vector<std::shared_ptr<Actor>>();
    std::unordered_map<std::string, Actor> templates = std::unordered_map<std::string, Actor>();
    rapidjson::Document initial_scene = rapidjson::Document();
    lua_State* lua_state;
};


#endif /* Scene_h */
