//
//  Image.h
//  game_engine
//
//  Created by Hadi Chaaban on 4/2/24.
//

#ifndef Image_h
#define Image_h

struct Image { //Performance considerations: functions are not DRY. Also note for later, at end of this assignment use C++ linter, TODO: Specialize data structures to work with specific requests, I.E. for Draw Pixel, priority que is unnecessary.
    static void Draw(std::string image_name, float x, float y) {
        DrawingRequest drawingRequest;
        drawingRequest.image_name = image_name;
        drawingRequest.x = x;
        drawingRequest.y = y;
        drawingRequest.layer = SceneLayer;
        
        Renderer::add_to_render_queue(std::make_shared<DrawingRequest>(drawingRequest));
    };
    
    static void DrawEx(std::string image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, float sorting_order) {
        DrawingRequest drawingRequest;
        drawingRequest.image_name = image_name;
        drawingRequest.x = x;
        drawingRequest.y = y;
        drawingRequest.rotation_degrees = int(rotation_degrees);
        drawingRequest.scale_x = scale_x;
        drawingRequest.scale_y = scale_y;
        drawingRequest.pivot_x = pivot_x;
        drawingRequest.pivot_y = pivot_y;
        drawingRequest.r = static_cast<Uint8>(r);
        drawingRequest.g = static_cast<Uint8>(g);
        drawingRequest.b = static_cast<Uint8>(b);
        drawingRequest.a = static_cast<Uint8>(a);
        drawingRequest.sorting_order = int(sorting_order);
        drawingRequest.layer = SceneLayer;
        
        Renderer::add_to_render_queue(std::make_shared<DrawingRequest>(drawingRequest));
    };
    
    static void DrawUI(std::string image_name, float x, float y) {
        DrawingRequest drawingRequest;
        drawingRequest.image_name = image_name;
        drawingRequest.x = x;
        drawingRequest.y = y;
        drawingRequest.layer = UILayer;
        Renderer::add_to_render_queue(std::make_shared<DrawingRequest>(drawingRequest));
    };
    
    static void DrawUIEx(std::string image_name, float x, float y, float r, float g, float b, float a, float sorting_order) {
        DrawingRequest drawingRequest;
        drawingRequest.image_name = image_name;
        drawingRequest.x = x;
        drawingRequest.y = y;
        drawingRequest.r = static_cast<Uint8>(r);
        drawingRequest.g = static_cast<Uint8>(g);
        drawingRequest.b = static_cast<Uint8>(b);
        drawingRequest.a = static_cast<Uint8>(a);
        drawingRequest.sorting_order = int(sorting_order);
        drawingRequest.layer = UILayer;
        Renderer::add_to_render_queue(std::make_shared<DrawingRequest>(drawingRequest));
    };
    
    static void DrawPixel(float x, float y, float r, float g, float b, float a) {
        DrawingRequest drawingRequest;
        drawingRequest.x = x;
        drawingRequest.y = y;
        drawingRequest.r = static_cast<Uint8>(r);
        drawingRequest.g = static_cast<Uint8>(g);
        drawingRequest.b = static_cast<Uint8>(b);
        drawingRequest.a = static_cast<Uint8>(a);
        drawingRequest.layer = PixelLayer;
        Renderer::add_to_render_queue(std::make_shared<DrawingRequest>(drawingRequest));
    };
};

#endif /* Image_h */
