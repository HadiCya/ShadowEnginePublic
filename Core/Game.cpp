//
//  Game.cpp
//  game_engine
//
//  Created by Hadi Chaaban on 3/28/24.
//

#include "Game.h"
#include "Rigidbody.h"
#include "Event.h"

void Game::BeginGame() {
    EngineUtils::ReadJsonFile("resources/game.config", config);
    this->game_title = config.HasMember("game_title") ? config["game_title"].GetString() : "";
    
    lua_state = luaL_newstate();
    luaL_openlibs(lua_state);
    
    //Classes and their properties
    luabridge::getGlobalNamespace(lua_state)//
        .beginClass<Actor>("Actor") //Exposed Actor Class
        .addFunction("GetName", &Actor::GetName)
        .addFunction("GetID", &Actor::GetID)
        .addFunction("GetComponentByKey", &Actor::GetComponentByKey)
        .addFunction("GetComponent", &Actor::GetComponent)
        .addFunction("GetComponents", &Actor::GetComponents)
        .addFunction("AddComponent", &Actor::AddComponentByType)
        .addFunction("RemoveComponent", &Actor::RemoveComponent)
        .endClass()
        .beginClass<glm::vec2>("vec2") //Exposed vec2 class
        .addProperty("x", &glm::vec2::x)
        .addProperty("y", &glm::vec2::y)
        .endClass()
        .beginClass<b2Vec2>("Vector2") //Exposed b2Vec2 class
        .addConstructor<void(*) (float, float)>()
        .addProperty("x", &b2Vec2::x)
        .addProperty("y", &b2Vec2::y)
        .addFunction("Normalize", &b2Vec2::Normalize)
        .addFunction("Length", &b2Vec2::Length)
        .addFunction("__add", &b2Vec2::operator_add)
        .addFunction("__sub", &b2Vec2::operator_sub)
        .addFunction("__mul", &b2Vec2::operator_mul)
        .addStaticFunction("Distance", &b2Distance)
        .addStaticFunction("Dot", static_cast<float (*)(const b2Vec2&, const b2Vec2&)>(&b2Dot))
        .endClass()
        .beginClass<Rigidbody>("Rigidbody") //Exposed Rigidbody Component class
        .addData("enabled", &Rigidbody::enabled)
        .addData("key", &Rigidbody::key)
        .addData("type", &Rigidbody::type)
        .addData("actor", &Rigidbody::actor)
        .addProperty("x", &Rigidbody::x)
        .addProperty("y", &Rigidbody::y)
        .addProperty("body_type", &Rigidbody::body_type)
        .addProperty("trigger_type", &Rigidbody::trigger_type)
        .addProperty("precise", &Rigidbody::precise)
        .addProperty("gravity_scale", &Rigidbody::gravity_scale)
        .addProperty("density", &Rigidbody::density)
        .addProperty("angular_friction", &Rigidbody::angular_friction)
        .addProperty("rotation", &Rigidbody::rotation)
        .addProperty("has_collider", &Rigidbody::has_collider)
        .addProperty("has_trigger", &Rigidbody::has_trigger)
        .addProperty("collider_type", &Rigidbody::collider_type)
        .addProperty("width", &Rigidbody::width)
        .addProperty("height", &Rigidbody::height)
        .addProperty("radius", &Rigidbody::radius)
        .addProperty("trigger_width", &Rigidbody::trigger_width)
        .addProperty("trigger_height", &Rigidbody::trigger_height)
        .addProperty("trigger_radius", &Rigidbody::trigger_radius)
        .addProperty("friction", &Rigidbody::friction)
        .addProperty("bounciness", &Rigidbody::bounciness)
        .addFunction("GetPosition", &Rigidbody::GetPosition)
        .addFunction("GetRotation", &Rigidbody::GetRotation)
        .addFunction("OnStart", &Rigidbody::OnStart)
        .addFunction("OnDestroy", &Rigidbody::OnDestroy)
        .addFunction("AddForce", &Rigidbody::AddForce)
        .addFunction("SetVelocity", &Rigidbody::SetVelocity)
        .addFunction("SetPosition", &Rigidbody::SetPosition)
        .addFunction("SetRotation", &Rigidbody::SetRotation)
        .addFunction("SetAngularVelocity", &Rigidbody::SetAngularVelocity)
        .addFunction("SetGravityScale", &Rigidbody::SetGravityScale)
        .addFunction("SetUpDirection", &Rigidbody::SetUpDirection)
        .addFunction("SetRightDirection", &Rigidbody::SetRightDirection)
        .addFunction("GetVelocity", &Rigidbody::GetVelocity)
        .addFunction("GetAngularVelocity", &Rigidbody::GetAngularVelocity)
        .addFunction("GetGravityScale", &Rigidbody::GetGravityScale)
        .addFunction("GetUpDirection", &Rigidbody::GetUpDirection)
        .addFunction("GetRightDirection", &Rigidbody::GetRightDirection)
        .endClass()
        .beginClass<Collision>("Collision") //Exposed Collision Class
        .addProperty("other", &Collision::other)
        .addProperty("point", &Collision::point)
        .addProperty("relative_velocity", &Collision::relative_velocity)
        .addProperty("normal", &Collision::normal)
        .endClass()
        .beginClass<HitResult>("HitResult") //Exposed HitResult Class
        .addProperty("actor", &HitResult::actor)
        .addProperty("point", &HitResult::point)
        .addProperty("normal", &HitResult::normal)
        .addProperty("is_trigger", &HitResult::is_trigger)
        .endClass();
    
    //Namespace APIs
    luabridge::getGlobalNamespace(lua_state)
        .beginNamespace("Debug") //DEBUG NAMESPACE API
        .addFunction("Log", CppLog)
        .addFunction("LogError", CppLogError)
        .endNamespace()
        .beginNamespace("Application") //APPLICATION NAMESPACE API
        .addFunction("Quit", QuitApplication)
        .addFunction("Sleep", SleepApplication)
        .addFunction("GetFrame", Renderer::GetFrameNumber)
        .addFunction("OpenURL", OpenURLApplication)
        .endNamespace()
        .beginNamespace("Input") //INPUT NAMESPACE API
        .addFunction("GetKey", Input::GetKey)
        .addFunction("GetKeyDown", Input::GetKeyDown)
        .addFunction("GetKeyUp", Input::GetKeyUp)
        .addFunction("GetMousePosition", Input::GetMousePosition)
        .addFunction("GetMouseButton", Input::GetMouseButton)
        .addFunction("GetMouseButtonDown", Input::GetMouseButtonDown)
        .addFunction("GetMouseButtonUp", Input::GetMouseButtonUp)
        .addFunction("GetMouseScrollDelta", Input::GetMouseScrollDelta)
        .endNamespace()
        .beginNamespace("Text") //TEXT NAMESPACE API
        .addFunction("Draw", Text::Draw)
        .endNamespace()
        .beginNamespace("Audio") //AUDIO NAMESPACE API
        .addFunction("Play", Audio::Play)
        .addFunction("Halt", Audio::Halt)
        .addFunction("SetVolume", Audio::SetVolume)
        .endNamespace()
        .beginNamespace("Actor") //ACTOR NAMESPACE API
        .addFunction("Find", FindActorByName)
        .addFunction("FindAll", FindActorsByName)
        .addFunction("Instantiate", InstantiateActor)
        .addFunction("Destroy", DestroyActor)
        .endNamespace()
        .beginNamespace("Image") //IMAGE NAMESPACE API
        .addFunction("Draw", Image::Draw)
        .addFunction("DrawEx", Image::DrawEx)
        .addFunction("DrawUI", Image::DrawUI)
        .addFunction("DrawUIEx", Image::DrawUIEx)
        .addFunction("DrawPixel", Image::DrawPixel)
        .endNamespace()
        .beginNamespace("Camera") //CAMERA NAMESPACE API
        .addFunction("SetPosition", Renderer::SetCameraPosition)
        .addFunction("GetPositionX", Renderer::GetCameraPositionX)
        .addFunction("GetPositionY", Renderer::GetCameraPositionY)
        .addFunction("SetZoom", Renderer::SetCameraZoom)
        .addFunction("GetZoom", Renderer::GetCameraZoom)
        .endNamespace()
        .beginNamespace("Scene") //SCENE NAMESPACE API
        .addFunction("Load", SetSceneChange)
        .addFunction("GetCurrent", GetCurrentScene)
        .addFunction("DontDestroy", DontDestroyActor)
        .endNamespace()
        .beginNamespace("Physics") //PHYSICS NAMESPACE API
        .addFunction("Raycast", Physics::Raycast)
        .addFunction("RaycastAll", Physics::RaycastAll)
        .endNamespace()
        .beginNamespace("Event")
        .addFunction("Publish", Event::Publish)
        .addFunction("Subscribe", Event::Subscribe)
        .addFunction("Unsubscribe", Event::Unsubscribe)
        .endNamespace();
    
    if(!config.HasMember("initial_scene")) {
        std::cout << "error: initial_scene unspecified";
        exit(0);
    }
    
    Component::lua_state = lua_state;
    if (std::filesystem::exists("resources/component_types")) {
        Component::load_components();
    }
    
    scene = Scene(config["initial_scene"].GetString(), lua_state);
    
    Physics physics = Physics();
    Renderer renderer = Renderer(game_title);
    
    scene.sort_actor_list();
    Input::Init();
    
    while (!done) {
        SDL_Event next_event;
        while (SDL_PollEvent(&next_event)) {
            if (next_event.type == SDL_QUIT) {
                done = true;
            } else {
                Input::ProcessEvent(next_event);
            }
        }
        update();
        Event::ProcessSubscriptions();
        physics.PhysicsStep();
        renderer.render_present();
        Input::LateUpdate();
    }
}

void Game::update() {
    if (change_scene != "") {
        change_scenes();
    }
    
    int actor_to_add_size = int(scene.actors_to_add.size());
    for (int i = 0; i < actor_to_add_size; i++) {
        std::shared_ptr<Actor> actor = scene.actors_to_add[i];
        scene.actors.push_back(actor);
        for (std::shared_ptr<Component> component : actor->on_start_components) { //PENDING_REFACTOR_CHECK
            processFunction("OnStart", actor, component);
        }
    }
    for (int i = 0; i < actor_to_add_size; i++) {
        scene.actors_to_add.erase(scene.actors_to_add.begin());
    }
    for (std::shared_ptr<Actor> actor : scene.actors) {
        for (std::shared_ptr<Component> component : actor->pending_components) {
            processFunction("OnStart", actor, component); //Potential issue with components creating new components OnStart
        }
        actor->addPendingComponents();
        actor->sort_component_list();
    }
    for (std::shared_ptr<Actor> actor : scene.actors) {
        for (std::shared_ptr<Component> component : actor->on_update_components) { //PENDING_REFACTOR_CHECK
            processFunction("OnUpdate", actor, component);
        }
    }
    for (std::shared_ptr<Actor> actor : scene.actors) {
        for (std::shared_ptr<Component> component : actor->on_late_update_components) { //PENDING_REFACTOR_CHECK
            processFunction("OnLateUpdate", actor, component);
        }
    }
    for (std::shared_ptr<Actor> actor : scene.actors) {
        if (!actor->pending_components_deletion.empty()) {
            std::sort(actor->pending_components_deletion.begin(), actor->pending_components_deletion.end(), [](std::shared_ptr<Component> a, std::shared_ptr<Component> b) {
                return a->getName() < b->getName();
            });
            for (luabridge::LuaRef component : actor->pending_components_deletion) {
                std::string comp_name = component["key"];
                std::shared_ptr<Component> wrapped_component = actor->components[comp_name];
                if (wrapped_component->onDestroy.has_value())
                    processFunction("OnDestroy", actor, wrapped_component);

                actor->components.erase(comp_name);
            }
            actor->pending_components_deletion.clear();
            actor->sort_component_list();
        }
    }
    scene.actors.erase(std::remove_if(scene.actors.begin(), scene.actors.end(),
                                      [&](const std::shared_ptr<Actor>& actor) {
        return ids_to_destroy.count(actor->GetID()) > 0;
    }),
                       scene.actors.end());
    
    scene.actors_to_add.erase(std::remove_if(scene.actors_to_add.begin(), scene.actors_to_add.end(),
                                             [&](const std::shared_ptr<Actor>& actor) {
        return ids_to_destroy.count(actor->GetID()) > 0;
    }),
                              scene.actors_to_add.end());
    ids_to_destroy.clear();
}

void Game::change_scenes() {
    std::vector<std::shared_ptr<Actor>> non_deleted_actors; //TODO: Actors arent properly deloaded, esp in physics
    for (std::shared_ptr<Actor> actor : scene.actors) {
        if (actor->dont_destroy) {
            non_deleted_actors.push_back(actor);
        }
    }
    
    scene = Scene(change_scene, lua_state);
    for (std::shared_ptr<Actor> actor : non_deleted_actors) {
        scene.actors.push_back(actor);
    }
    
    scene.sort_actor_list();
    change_scene = "";
}
