//
//  Scene.cpp
//  game_engine
//
//  Created by Hadi Chaaban on 1/31/24.
//

#include "Scene.h"
#include <filesystem>
#include <iostream>
#include "EngineUtils.h"
#include <vector> 

Scene::Scene(std::string scene_name, lua_State* lua_state) {
    this->lua_state = lua_state;
    this->scene_name = scene_name;
    scene_exists(scene_name);
    if (std::filesystem::exists("resources/actor_templates")) {
        load_templates();
    }
    EngineUtils::ReadJsonFile("resources/scenes/" + scene_name + ".scene", initial_scene);
    load_actors();
}

Scene::Scene() {}

void Scene::scene_exists(const std::string& name) {
    if (!std::filesystem::exists("resources/scenes/" + name + ".scene")) {
        std::cout << "error: scene " << name << " is missing";
        exit(0);
    }
}

void Scene::load_templates() {
    for(auto& p: std::filesystem::directory_iterator("resources/actor_templates")) {
        auto filename = p.path().filename().string();
        rapidjson::Document temp_template;
        EngineUtils::ReadJsonFile(p.path().string(), temp_template);
        
        Actor actor = Actor();

        if (temp_template.HasMember("name"))
            actor.setName(temp_template["name"].GetString());
        
        if (temp_template.HasMember("components") && temp_template["components"].IsObject()) {
            rapidjson::GenericObject temp_components = temp_template["components"].GetObject();
            for (const auto & [ key, value ] : temp_components) {
                std::string component_type_name = value.GetObject()["type"].GetString();
                Component::component_exists(component_type_name);
                actor.addComponent(std::make_shared<Component>(key.GetString(), component_type_name, value.GetObject()), &actor);
            }
        }
        
        std::string extension = ".template";
        if(filename.size() >= extension.size() && filename.substr(filename.size() - extension.size()) == extension) {
            filename = filename.substr(0, filename.size() - extension.size());
        }
        
        templates.insert(std::make_pair(filename, actor));
    }
}

void Scene::load_actors() {
    rapidjson::GenericArray temp_actors = initial_scene["actors"].GetArray();
    for (int i = 0; i < temp_actors.Size(); i++) {
        if (temp_actors[i].IsObject()) {
            auto& actor_data = temp_actors[i];
            
            Actor template_actor;
            if (actor_data.HasMember("template")) {
                std::string template_name = actor_data["template"].GetString();
                if (templates.find(template_name) != templates.end()) {
                    template_actor = templates[template_name];
                } else {
                    std::cout << "error: template " << template_name << " is missing";
                    exit(0);
                }
            }
            
            Actor actor;
            actor.lua_state = lua_state;

            if (template_actor.name != "") {
                actor = template_actor;
            }
            std::shared_ptr<Actor> shared_actor = std::make_shared<Actor>(actor);

            if (actor_data.HasMember("name"))
                shared_actor->setName(actor_data["name"].GetString());
            
            if (actor_data.HasMember("components") && actor_data["components"].IsObject()) {
                rapidjson::GenericObject temp_components = actor_data["components"].GetObject();
                for (const auto & [ key, value ] : temp_components) {
                    if (template_actor.name != "" && template_actor.components.find(key.GetString()) != template_actor.components.end()) {
                        Component component = Component(key.GetString(), shared_actor->components[key.GetString()]->getType(), value.GetObject());
                        shared_actor->addComponent(std::make_shared<Component>(component), shared_actor.get());
                    } else {
                        std::string component_type_name = value.GetObject()["type"].GetString();
                        Component::component_exists(component_type_name);
                        
                        Component component = Component(key.GetString(), component_type_name, value.GetObject());
                        shared_actor->addComponent(std::make_shared<Component>(component), shared_actor.get());
                    }
                }
            }
            shared_actor->order = Actor::global_actor_order;
            Actor::global_actor_order++;
            shared_actor->sort_component_list();
            
            this->actors_to_add.push_back(shared_actor);
        }
    }
}
