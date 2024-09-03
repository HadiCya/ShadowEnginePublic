//
//  Audio.h
//  game_engine
//
//  Created by Hadi Chaaban on 4/2/24.
//

#ifndef Audio_h
#define Audio_h

#include "EngineUtils.h"
#include "SDL2_mixer/SDL_mixer.h"

struct Audio {
    static void Play(int channel, std::string clip_name, bool does_loop) {
        std::string clip_name_file = "resources/audio/" + clip_name + EngineUtils::audio_exists(clip_name);

        Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

        Mix_Chunk* mix_chunk = Mix_LoadWAV(clip_name_file.c_str());
                            
        Mix_PlayChannel(channel, mix_chunk, does_loop ? -1 : 0);
    };
    
    static void Halt(int channel) {
        Mix_HaltChannel(channel);
    };
    
    static void SetVolume(int channel, float volume) {
        Mix_Volume(channel, static_cast<Uint8>(volume));
    };
};

#endif /* Audio_h */
