//
//  Event.h
//  game_engine
//
//  Created by Hadi Chaaban on 4/7/24.
//

#ifndef Event_h
#define Event_h

#include "Component.h"

struct SubscriberComparator {
    luabridge::LuaRef component;
    luabridge::LuaRef function;

    SubscriberComparator(const luabridge::LuaRef& comp, const luabridge::LuaRef& func)
        : component(comp), function(func) {}

    bool operator()(const std::pair<luabridge::LuaRef, luabridge::LuaRef>& sub) const {
        return sub.first == component && sub.second == function;
    }
};

class Event {
public:
    static void Publish(std::string event_type, luabridge::LuaRef event_object) {
        for (auto [ component, function ] : subscribers[event_type]) {
            function(component, event_object);
        }
    };
    
    static void Subscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function) {
        pending_subscribers[event_type].push_back(std::pair<luabridge::LuaRef, luabridge::LuaRef>(component, function));
    };
    
    static void Unsubscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function) {
        pending_unsubscribers[event_type].push_back(std::pair<luabridge::LuaRef, luabridge::LuaRef>(component, function));
    };
    
    static void ProcessSubscriptions() {
        for (auto & [ event_type, pending_list ] : pending_subscribers ) {
            for (auto & [ component, function ] : pending_list) {
                subscribers[event_type].push_back(std::pair<luabridge::LuaRef, luabridge::LuaRef>(component, function));
            }
            pending_list.clear();
        }
        for (auto & [ event_type, pending_list ] : pending_unsubscribers ) {
            auto& subs = subscribers[event_type];
            for (auto & [ component, function ] : pending_list) {
                subs.erase(std::remove_if(subs.begin(), subs.end(),
                    SubscriberComparator(component, function)),
                subs.end());
            }
            pending_list.clear();
        }
    };
    
    static inline std::unordered_map<std::string, std::vector<std::pair<luabridge::LuaRef, luabridge::LuaRef>>> subscribers, pending_subscribers, pending_unsubscribers;
};

#endif /* Event_h */
