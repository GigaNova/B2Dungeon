//
// Created by styn2 on 11/29/2019.
//

#ifndef B2BSAND_GAME_H
#define B2BSAND_GAME_H

#include "Context.h"

void updateEntities(Context* _game);
void drawMap(SDL_Renderer* _renderer, Context *_game);
void drawTile(SDL_Renderer* _renderer, Tile* _tile);
void drawEntities(SDL_Renderer* _renderer, Context* _game);

#endif //B2BSAND_GAME_H
