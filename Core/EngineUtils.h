//
//  EngineUtils.h
//  game_engine
//
//  Created by Hadi Chaaban on 1/30/24.
//

#ifndef EngineUtils_h
#define EngineUtils_h

#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <iostream>

#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"

enum NodeType {
    StringNodeType,
    IntNodeType,
    FloatNodeType,
    BoolNodeType
};

using NodeValue = std::variant<std::string, int, float, bool>;

struct Node {
    NodeValue val;
    NodeType nodeType;

    Node() {};
    Node(NodeValue val, NodeType nodeType) : val(val), nodeType(nodeType) {};
};

class EngineUtils {
public:
    
    static void ReadJsonFile(const std::string& path, rapidjson::Document & out_document)
    {
        FILE* file_pointer = nullptr;
#ifdef _WIN32
        fopen_s(&file_pointer, path.c_str(), "rb");
#else
        file_pointer = fopen(path.c_str(), "rb");
#endif
        char buffer[65536];
        rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
        out_document.ParseStream(stream);
        std::fclose(file_pointer);

        if (out_document.HasParseError()) {
            rapidjson::ParseErrorCode errorCode = out_document.GetParseError();
            std::cout << "error parsing json at [" << path << "]" << std::endl;
            exit(0);
        }
    }
    
    static std::string obtain_word_after_phrase(const std::string& input, const std::string& phrase) {
        size_t pos = input.find(phrase);
        
        if (pos == std::string::npos) return "";
        
        pos += phrase.length();
        while (pos < input.size() && std::isspace(input[pos])) {
            ++pos;
        }
        
        if (pos == input.size()) return "";
        
        size_t endPos = pos;
        while (endPos < input.size() && !std::isspace(input[endPos])) {
            ++endPos;
        }
        
        return input.substr(pos, endPos - pos);
    }
    
    static void path_exists(const std::string& name) {
        if (!std::filesystem::exists(name)) {
            std::cout << "error: " << name << " missing";
            exit(0);
        }
    }
    
    static void image_exists(const std::string& name) {
        if (!std::filesystem::exists("resources/images/" + name + ".png")) {
            std::cout << "error: missing image " << name;
            exit(0);
        }
    }
    
    static void font_exists(const std::string& name) {
        if (!std::filesystem::exists("resources/fonts/" + name + ".ttf")) {
            std::cout << "error: font " << name << " missing";
            exit(0);
        }
    }
    
    static std::string audio_exists(const std::string& name) {
        if (std::filesystem::exists("resources/audio/" + name + ".wav")) {
            return ".wav";
        } else if (std::filesystem::exists("resources/audio/" + name + ".ogg")) {
            return ".ogg";
        } else {
            std::cout << "error: failed to play audio clip " << name;
            exit(0);
        }
    }
};


#endif /* EngineUtils_h */
