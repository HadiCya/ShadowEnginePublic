//
//  DrawingRequest.h
//  game_engine
//
//  Created by Hadi Chaaban on 4/2/24.
//

#ifndef DrawingRequest_h
#define DrawingRequest_h

#include <optional>
#include "RenderLayer.h"

static size_t next_sequence = 0;

struct DrawingRequest { //TODO: Consider creating different specialized requests to save memory
public:
    DrawingRequest() : sequence(next_sequence++) {};
    std::string image_name;
    float x;
    float y;
    int rotation_degrees = 0;
    float scale_x = 1.0f;
    float scale_y = 1.0f;
    float pivot_x = 0.5f;
    float pivot_y = 0.5f;
    int r = 255;
    int g = 255;
    int b = 255;
    int a = 255;
    int sorting_order = 0;
    TTF_Font* font;
    std::string str_content;
    size_t sequence;
    RenderLayer layer;
    
    bool operator<(const DrawingRequest& other) const {
        if (sorting_order == other.sorting_order)
            return sequence < other.sequence;
        return sorting_order < other.sorting_order;
    }
};

struct CompareDrawingRequest {
    bool operator()(const std::shared_ptr<DrawingRequest>& lhs, const std::shared_ptr<DrawingRequest>& rhs) const {
        return *lhs < *rhs;
    }
};

#endif /* DrawingRequest_h */
