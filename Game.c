#include "Game.h"
#include "stdio.h"
#include "Systems.h"

void updateEntities(Context *_game) {
    EntityData* data = &_game->entityData;

    for(uint32_t i = 0; i < data->highestId; ++i){
        if(data->id[i] == FREE_ID) continue;

        inputSystem(data, &data->inputListeners[i], data->velocity[i]);
        collisionSystem(data, &data->collider[i], &data->positions[i], &data->dimensions[i], data->velocity[i]);
        velocitySystem(&data->positions[i], data->velocity[i]);
    }
}

void drawMap(SDL_Renderer* _renderer, Context* _game) {
    Map* map = _game->map;

    drawTile(_renderer, map->root);

    resetMap(map);
}

void drawTile(SDL_Renderer* _renderer, Tile* _tile) {
    drawSystem(_renderer, &_tile->pos, NULL, &_tile->sprite, false);
    _tile->drawn = true;

    if(_tile->up && !_tile->up->drawn){
        drawTile(_renderer, _tile->up);
    }
    if(_tile->right && !_tile->right->drawn){
        drawTile(_renderer, _tile->right);
    }
    if(_tile->down && !_tile->down->drawn){
        drawTile(_renderer, _tile->down);
    }
    if(_tile->left && !_tile->left->drawn){
        drawTile(_renderer, _tile->left);
    }
}

void drawEntities(SDL_Renderer* _renderer, Context *_game) {
    const EntityData* data = &_game->entityData;

    for(uint32_t i = 0; i < data->highestId; ++i){
        if(data->id[i] == FREE_ID || data->sprites[i] == NULL) continue;

        drawSystem(_renderer, &data->positions[i], &data->rotation[i], data->sprites[i], true);
    }
}