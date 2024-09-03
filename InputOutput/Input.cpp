//
//  Input.cpp
//  game_engine
//
//  Created by Hadi Chaaban on 3/13/24.
//

#include "Input.h"
#include <iostream>

void Input::Init() {
    for (int code = SDL_SCANCODE_UNKNOWN; code < SDL_NUM_SCANCODES; code++) {
        keyboard_states[static_cast<SDL_Scancode>(code)] = INPUT_STATE_UP;
    }
    for (int i = 1; i <= 3; i++) {
        mouse_button_states[i] = INPUT_STATE_UP;
    }
}

void Input::ProcessEvent(const SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_DOWN;
        just_became_down_scancodes.push_back(e.key.keysym.scancode);
    } else if (e.type == SDL_KEYUP) {
        keyboard_states[e.key.keysym.scancode] = INPUT_STATE_JUST_BECAME_UP;
        just_became_up_scancodes.push_back(e.key.keysym.scancode);
    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
        mouse_button_states[static_cast<int>(e.button.button)] = INPUT_STATE_JUST_BECAME_DOWN;
        just_became_down_buttons.push_back(static_cast<int>(e.button.button));
    } else if (e.type == SDL_MOUSEBUTTONUP) {
        mouse_button_states[static_cast<int>(e.button.button)] = INPUT_STATE_JUST_BECAME_UP;
        just_became_up_buttons.push_back(static_cast<int>(e.button.button));
    } else if (e.type == SDL_MOUSEMOTION) {
        mouse_position = glm::vec2(e.motion.x, e.motion.y);
    } else if (e.type == SDL_MOUSEWHEEL) {
        mouse_scroll_this_frame = e.wheel.preciseY;
    }
}

void Input::LateUpdate() {
    for (const SDL_Scancode& code : just_became_down_scancodes) {
        keyboard_states[code] = INPUT_STATE_DOWN;
    }
    for (int button : just_became_down_buttons) {
        mouse_button_states[button] = INPUT_STATE_DOWN;
    }
    just_became_down_scancodes.clear();
    just_became_down_buttons.clear();
    
    for (const SDL_Scancode& code : just_became_up_scancodes) {
        keyboard_states[code] = INPUT_STATE_UP;
    }
    for (int button : just_became_up_buttons) {
        mouse_button_states[button] = INPUT_STATE_UP;
    }
    just_became_up_scancodes.clear();
    just_became_up_buttons.clear();
    mouse_scroll_this_frame = 0;
}

bool Input::GetKey(std::string keycode) {
    auto it = __keycode_to_scancode.find(keycode);
    if (it == __keycode_to_scancode.end()) {
        return false;
    }
    SDL_Scancode scancode = it->second;
    return keyboard_states[scancode] == INPUT_STATE_DOWN || keyboard_states[scancode] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyDown(std::string keycode) {
    auto it = __keycode_to_scancode.find(keycode);
    if (it == __keycode_to_scancode.end()) {
        return false;
    }
    SDL_Scancode scancode = it->second;
    return keyboard_states[scancode] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyUp(std::string keycode) {
    auto it = __keycode_to_scancode.find(keycode);
    if (it == __keycode_to_scancode.end()) {
        return false;
    }
    SDL_Scancode scancode = it->second;
    return keyboard_states[scancode] == INPUT_STATE_JUST_BECAME_UP;
}

glm::vec2 Input::GetMousePosition() {
    return mouse_position;
}

bool Input::GetMouseButton(int button_num) {
    return mouse_button_states[button_num] == INPUT_STATE_DOWN || mouse_button_states[button_num] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetMouseButtonDown(int button_num) {
    return mouse_button_states[button_num] == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetMouseButtonUp(int button_num) {
    return mouse_button_states[button_num] == INPUT_STATE_JUST_BECAME_UP;
}

float Input::GetMouseScrollDelta() {
    return mouse_scroll_this_frame;
}
