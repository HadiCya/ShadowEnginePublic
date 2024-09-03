//
//  main.cpp
//  game_engine
//
//  Created by Hadi Chaaban on 1/22/24.
//

#include "Game.h"

int main(int argc, const char * argv[]) {
    EngineUtils::path_exists("resources/");
    EngineUtils::path_exists("resources/game.config");
    
    Game& game = Game::getInstance();;
    game.BeginGame();
    
    return 0;
}
