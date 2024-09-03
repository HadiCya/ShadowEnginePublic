//
//  Renderer.h
//  game_engine
//
//  Created by Hadi Chaaban on 4/2/24.
//

#ifndef Renderer_h
#define Renderer_h

#include "SDL2/SDL.h"
#include "SDL2_ttf/SDL_ttf.h"
#include "DrawingRequest.h"
#include "RenderLayer.h"

class Renderer {
public:
    Renderer(std::string game_title) {
        load_config();
        
        SDL_Init(SDL_INIT_VIDEO);
        TTF_Init();
        
        SDL_Window* window = SDL_CreateWindow(game_title.c_str(),
                                                         1000, 1000,
                                                         this->viewport_size.x, this->viewport_size.y,
                                                         SDL_WINDOW_SHOWN);
        
        this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
        
        if (std::filesystem::exists("resources/images"))
            load_images();
        
        SDL_SetRenderDrawColor(renderer, this->clear_color.r, this->clear_color.g, this->clear_color.b, 255);
        SDL_RenderClear(renderer);
        
        current_frame_start_timestamp = SDL_GetTicks();
        frame_number = 0;
    };
    
    void load_config() {
        if (std::filesystem::exists("resources/rendering.config")) {
            EngineUtils::ReadJsonFile("resources/rendering.config", rendering);
            this->viewport_size.x = rendering.HasMember("x_resolution") ? rendering["x_resolution"].GetInt() : 640;
            this->viewport_size.y = rendering.HasMember("y_resolution") ? rendering["y_resolution"].GetInt() : 360;
            this->clear_color.r = rendering.HasMember("clear_color_r") ? rendering["clear_color_r"].GetInt() : 255;
            this->clear_color.g = rendering.HasMember("clear_color_g") ? rendering["clear_color_g"].GetInt() : 255;
            this->clear_color.b = rendering.HasMember("clear_color_b") ? rendering["clear_color_b"].GetInt() : 255;
            this->cam_offset.x = rendering.HasMember("cam_offset_x") ? rendering["cam_offset_x"].GetFloat() : 0.0f;
            this->cam_offset.y = rendering.HasMember("cam_offset_y") ? rendering["cam_offset_y"].GetFloat() : 0.0f;
            this->zoom_factor = rendering.HasMember("zoom_factor") ? rendering["zoom_factor"].GetFloat() : 1.0f;
        }
    };
    
    void load_images() {
        for(auto& p: std::filesystem::directory_iterator("resources/images")) {
            auto filename = p.path().filename().string();
            
            std::string extension = ".png";
            if(filename.size() >= extension.size() && filename.substr(filename.size() - extension.size()) == extension) {
                filename = filename.substr(0, filename.size() - extension.size());
            }
            SDL_Texture* image_texture = IMG_LoadTexture(Renderer::renderer, ("resources/images/" + filename + ".png").c_str());
            images.insert(std::make_pair(filename, image_texture));
        }
    };
    
    static void add_to_render_queue(std::shared_ptr<DrawingRequest> drawingRequest) {
        switch(drawingRequest->layer) {
            case SceneLayer:
                drawing_requests_scene.push_back(drawingRequest);
                break;
            case UILayer:
                drawing_requests_UI.push_back(drawingRequest);
                break;
            case TextLayer:
                drawing_requests_text.push_back(drawingRequest);
                break;
            case PixelLayer:
                drawing_requests_pixel.push_back(drawingRequest);
                break;
        }
        
    }
    
    void render_present() {
        SDL_SetRenderDrawColor(renderer, clear_color.r, clear_color.g, clear_color.b, 255);
        SDL_RenderClear(renderer);
        SDL_RenderSetScale(renderer, zoom_factor, zoom_factor);
        std::sort(drawing_requests_scene.begin(), drawing_requests_scene.end(), CompareDrawingRequest());
        for (std::shared_ptr<DrawingRequest> drawingRequest : drawing_requests_scene) {
            renderSceneObject(drawingRequest);
        }
        drawing_requests_scene.clear();
        SDL_RenderSetScale(renderer, 1.0, 1.0);
        std::sort(drawing_requests_UI.begin(), drawing_requests_UI.end(), CompareDrawingRequest());
        for (std::shared_ptr<DrawingRequest> drawingRequest : drawing_requests_UI) {
            renderUIObject(drawingRequest);
        }
        drawing_requests_UI.clear();
        std::sort(drawing_requests_text.begin(), drawing_requests_text.end(), CompareDrawingRequest());
        for (std::shared_ptr<DrawingRequest> drawingRequest : drawing_requests_text) {
            renderTextObject(drawingRequest);
        }
        drawing_requests_text.clear();
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        std::sort(drawing_requests_pixel.begin(), drawing_requests_pixel.end(), CompareDrawingRequest());
        for (std::shared_ptr<DrawingRequest> drawingRequest : drawing_requests_pixel) {
            renderPixelObject(drawingRequest);
        }
        drawing_requests_pixel.clear();
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        SDL_RenderPresent(renderer);
        SDL_Delay();
        frame_number++;
    }
    
    void renderSceneObject(std::shared_ptr<DrawingRequest> drawingRequest) {
        SDL_Texture* image_texture = images[drawingRequest->image_name];//
        SDL_SetTextureColorMod(image_texture, drawingRequest->r, drawingRequest->g, drawingRequest->b);
        SDL_SetTextureAlphaMod(image_texture, drawingRequest->a);
        SDL_Point info = getsize(image_texture);
        
        SDL_Point point;
        point.x = static_cast<int>(drawingRequest->pivot_x * info.x * drawingRequest->scale_x);
        point.y = static_cast<int>(drawingRequest->pivot_y * info.y * drawingRequest->scale_y);
        
        SDL_Rect rect;
        rect.x = static_cast<int>((drawingRequest->x - camera_center.x) * scene_unit + (viewport_size.x / zoom_factor / 2) - point.x);
        rect.y = static_cast<int>((drawingRequest->y - camera_center.y) * scene_unit + (viewport_size.y / zoom_factor / 2) - point.y);
        rect.w = info.x * drawingRequest->scale_x;
        rect.h = info.y * drawingRequest->scale_y;
        
        SDL_RenderCopyEx(renderer, image_texture, NULL, &rect, static_cast<int>(drawingRequest->rotation_degrees), &point, SDL_FLIP_NONE);
        
        SDL_SetTextureColorMod(image_texture, 255, 255, 255);
        SDL_SetTextureAlphaMod(image_texture, 255);
    }
    
    void renderUIObject(std::shared_ptr<DrawingRequest> drawingRequest)  {
        SDL_Texture* image_texture = images[drawingRequest->image_name];
        SDL_SetTextureColorMod(image_texture, drawingRequest->r, drawingRequest->g, drawingRequest->b);
        SDL_SetTextureAlphaMod(image_texture, drawingRequest->a);
        SDL_Point info = getsize(image_texture);

        SDL_Rect rect;
        rect.x = drawingRequest->x;
        rect.y = drawingRequest->y;
        rect.w = info.x;
        rect.h = info.y;
        
        SDL_RenderCopy(renderer, image_texture, NULL, &rect);
        
        SDL_SetTextureColorMod(image_texture, 255, 255, 255);
        SDL_SetTextureAlphaMod(image_texture, 255);
    }
    
    void renderTextObject(std::shared_ptr<DrawingRequest> drawingRequest) {
        SDL_Color color = {static_cast<Uint8>(drawingRequest->r), static_cast<Uint8>(drawingRequest->g), static_cast<Uint8>(drawingRequest->b), static_cast<Uint8>(drawingRequest->a)};
        SDL_Surface* textSurface = TTF_RenderText_Solid(drawingRequest->font, drawingRequest->str_content.c_str(), color);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(Renderer::renderer, textSurface);
        SDL_Rect textRect = {static_cast<int>(drawingRequest->x), static_cast<int>(drawingRequest->y), textSurface->w, textSurface->h};
        SDL_FreeSurface(textSurface);

        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    }
    
    void renderPixelObject(std::shared_ptr<DrawingRequest> drawingRequest) {
        SDL_SetRenderDrawColor(renderer, drawingRequest->r, drawingRequest->g, drawingRequest->b, drawingRequest->a);
        SDL_RenderDrawPoint(renderer, int(drawingRequest->x), int(drawingRequest->y));
    }
    
    static void SetCameraPosition(float x, float y) {
        camera_center = glm::vec2(x, y);
    }
    
    static float GetCameraPositionX() {
        return camera_center.x;
    }
    
    static float GetCameraPositionY() {
        return camera_center.y;
    }
    
    static void SetCameraZoom(float camera_zoom) {
        zoom_factor = camera_zoom;
    }
    
    static float GetCameraZoom() {
        return zoom_factor;
    }
    
    static void SDL_Delay() { //Hard set to aim for 60fps
        Uint32 current_frame_end_timestamp = SDL_GetTicks();
        Uint32 current_frame_duration_milliseconds = current_frame_end_timestamp - current_frame_start_timestamp;
        Uint32 desired_frame_duration_milliseconds = 16;

        int delay_ticks = std::max(static_cast<int>(desired_frame_duration_milliseconds) - static_cast<int>(current_frame_duration_milliseconds), 1);

        ::SDL_Delay(delay_ticks);
        
        current_frame_start_timestamp = SDL_GetTicks();
    }
    
    static inline int frame_number = 0;
    static inline Uint32 current_frame_start_timestamp = 0;
    static int GetFrameNumber() { return frame_number; }
    
    static SDL_Point getsize(SDL_Texture *texture) {
        SDL_Point size;
        SDL_QueryTexture(texture, NULL, NULL, &size.x, &size.y);
        return size;
    }
    
    rapidjson::Document rendering = rapidjson::Document();
    static inline float zoom_factor = 1.0f;
    glm::ivec2 viewport_size = glm::ivec2(640, 360);
    static inline glm::vec2 camera_center;
    glm::ivec3 clear_color = glm::ivec3(255, 255, 255);
    glm::vec2 cam_offset = glm::vec2(0.0f, 0.0f);
    static inline SDL_Renderer* renderer;
    static inline std::vector<std::shared_ptr<DrawingRequest>> drawing_requests_scene, drawing_requests_UI, drawing_requests_text, drawing_requests_pixel;
    int scene_unit = 100;
    std::unordered_map<std::string, SDL_Texture*> images;
};

#endif /* Renderer_h */
