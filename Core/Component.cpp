//
//  Component.cpp
//  game_engine
//
//  Created by Hadi Chaaban on 3/27/24.
//

#include "Component.h"
#include <cstring>
#include <string>
#include "Rigidbody.h"

Component::Component(const std::string& name, const std::string& type, rapidjson::Value::ConstObject variables) : table(nullptr){
    this->name = name;
    this->type = type;

    if (type == "Rigidbody") {
        luabridge::LuaRef componentRef(lua_state, Rigidbody());
        table = componentRef;
        table["key"] = name;
    } else {
        table = luabridge::newTable(lua_state);
        table["key"] = name;
        table["enabled"] = true;

        luabridge::LuaRef new_metatable = luabridge::newTable(lua_state);
        new_metatable["__index"] = component_templates[type]->table;
        
        table.push(lua_state);
        new_metatable.push(lua_state);
        lua_setmetatable(lua_state, -2);
        lua_pop(lua_state, 1);
    }
    
    for (const auto& [key, value] : variables) {
        std::string temp_key = key.GetString();
        luabridge::LuaRef existingValue = table[temp_key.c_str()];
        
        if (!existingValue.isNil()) {
            if (value.IsString()) {
                table[temp_key.c_str()] = value.GetString();
                this->variables[temp_key] = Node(value.GetString(), StringNodeType);
            } else if (value.IsInt()) {
                table[temp_key.c_str()] = value.GetInt();
                this->variables[temp_key] = Node(value.GetInt(), IntNodeType);
            } else if (value.IsBool()) {
                table[temp_key.c_str()] = value.GetBool();
                this->variables[temp_key] = Node(value.GetBool(), BoolNodeType);
            } else if (value.IsDouble()) {
                table[temp_key.c_str()] = value.GetDouble();
                this->variables[temp_key] = Node(value.GetFloat(), FloatNodeType);
            }
        }
    }
    
    grabLifeCycle();
}

Component::Component(const std::string& name, const std::string& type) : table(nullptr) {
    this->name = name;
    this->type = type;

    if (type == "Rigidbody") {
        luabridge::LuaRef componentRef(lua_state, Rigidbody());
        table = componentRef;
        table["key"] = name;
    } else {
        table = luabridge::newTable(lua_state);
        table["key"] = name;
        table["enabled"] = true;

        luabridge::LuaRef new_metatable = luabridge::newTable(lua_state);
        new_metatable["__index"] = component_templates[type]->table;
        
        table.push(lua_state);
        new_metatable.push(lua_state);
        lua_setmetatable(lua_state, -2);
        lua_pop(lua_state, 1);
    }
    
    grabLifeCycle();
}

Component::Component(const std::string& type) : table(nullptr) {
    this->name = type;
    this->type = type;
    
    std::string file_path = ("resources/component_types/" + type + ".lua");
    if (luaL_dofile(lua_state, file_path.c_str()) == LUA_OK) {
        table = luabridge::getGlobal(lua_state, type.c_str());
        grabLifeCycle();
    } else {
        std::cout << "problem with lua file " << type;
        exit(0);
    }
}

Component::Component(std::shared_ptr<Component> otherComponent) {
    name = otherComponent->name;
    type = otherComponent->type;
    
    if (type == "Rigidbody") {
        luabridge::LuaRef componentRef(lua_state, Rigidbody());
        table = componentRef;
        table["key"] = name;
    } else {
        table = luabridge::newTable(lua_state);
        table["key"] = name;
        table["enabled"] = true;

        luabridge::LuaRef new_metatable = luabridge::newTable(lua_state);
        new_metatable["__index"] = component_templates[type]->table;
        
        table.push(lua_state);
        new_metatable.push(lua_state);
        lua_setmetatable(lua_state, -2);
        lua_pop(lua_state, 1);
    }
    
    variables = otherComponent->variables;
    if (!variables.empty()) {
        for (const auto& [key, value] : variables) {
            std::string temp_key = key;
            luabridge::LuaRef existingValue = table[temp_key.c_str()];
            switch(value.nodeType) {
                case StringNodeType:
                    table[temp_key.c_str()] = std::get<std::string>(value.val);
                    break;
                case IntNodeType:
                    table[temp_key.c_str()] = std::get<int>(value.val);
                    break;
                case BoolNodeType:
                    table[temp_key.c_str()] = std::get<bool>(value.val);
                    break;
                case FloatNodeType:
                    table[temp_key.c_str()] = std::get<float>(value.val);
                    break;
            }
        }
    }
    grabLifeCycle();
}
