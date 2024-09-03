//
//  Text.h
//  game_engine
//
//  Created by Hadi Chaaban on 4/2/24.
//

#ifndef Text_h
#define Text_h

#include "SDL2_ttf/SDL_ttf.h"
#include <string>
#include "Renderer.h"

struct Text {
    static void Draw(std::string str_content, float x, float y, std::string font_name, float font_size, float r, float g, float b, float a) {
        TTF_Font* font = nullptr;
        auto font_family = font_map.find(font_name);
        if (font_family != font_map.end()) {
            auto font_entry = font_family->second.find(int(font_size));
            if (font_entry != font_family->second.end()) {
                font = font_entry->second;
            }
        }
        
        if (!font) {
            font = TTF_OpenFont(("resources/fonts/" + font_name + ".ttf").c_str(), int(font_size));
            font_map[font_name][int(font_size)] = font;
        }
        
        DrawingRequest drawingRequest;
        drawingRequest.str_content = str_content;
        drawingRequest.x = x;
        drawingRequest.y = y;
        drawingRequest.font = font;
        drawingRequest.r = static_cast<Uint8>(r);
        drawingRequest.g = static_cast<Uint8>(g);
        drawingRequest.b = static_cast<Uint8>(b);
        drawingRequest.a = static_cast<Uint8>(a);
        drawingRequest.layer = TextLayer;
        Renderer::add_to_render_queue(std::make_shared<DrawingRequest>(drawingRequest));
    };
    
    static inline std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> font_map;
};


#endif /* Text_h */
