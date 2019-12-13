#include <stdint.h>
#include <stdlib.h>
#include "Map.h"

Map* createMap(SDL_Renderer* _renderer, uint16_t _width, uint16_t _height) {
    Tile* linker[_width][_height];

    Map* map = (Map*)malloc(sizeof(Map));
    map->root = createTile(_renderer, 0, 0, 1, 1);
    linker[0][0] = map->root;

    Tile* currentX = map->root;
    //Build tiles
    for(uint16_t x = 0; x < _width; ++x){
        for(uint16_t y = 0; y < _height; ++y){
            if(x == 0 && y == 0) continue;

            Tile* yTile = createTile(_renderer, x, y, 1, 1);
            insertTileY(currentX, yTile);
            linker[x][y] = yTile;
        }

        if(x + 1 >= _width) break;

        Tile* xTile = createTile(_renderer, (x + 1), 0, 1, 1);
        insertTileX(currentX, xTile);
        currentX = xTile;
        linker[x][0] = currentX;
    }

    //Link tiles
    for(uint16_t x = 0; x < _width; ++x) {
        for (uint16_t y = 0; y < _height; ++y) {
            Tile* tile = linker[x][y];
            if(x - 1 >= 0){
                tile->left = linker[x - 1][y];
            }
            if(x + 1 < _width){
                tile->right = linker[x + 1][y];
            }
            if(y - 1 >= 0){
                tile->up = linker[x][y - 1];
            }
        }
    }

    return map;
}

void insertTileX(Tile* _tile, Tile* _toBeInserted) {
    if(_tile->right){
        insertTileX((Tile*)_tile->right, _toBeInserted);
    }
    else{
        _tile->right = (struct Tile*)_toBeInserted;
    }
}

void insertTileY(Tile* _tile, Tile* _toBeInserted) {
    if(_tile->down){
        insertTileY((Tile*)_tile->down, _toBeInserted);
    }
    else{
        _tile->down = (struct Tile*)_toBeInserted;
    }
}

Tile* createTile(SDL_Renderer* _renderer, uint16_t _x, uint16_t _y, uint16_t _offsetX, uint16_t _offsetY) {
    Tile* tile = (Tile*)malloc(sizeof(Tile));
    tile->up = NULL;
    tile->right = NULL;
    tile->down = NULL;
    tile->left = NULL;

    Sprite sprite;
    loadSprite(_renderer, &sprite, "testmap.png");
    sprite.height = TILE_HEIGHT;
    sprite.width = TILE_WIDTH;
    sprite.clip_x = _offsetX + 1 + (_offsetX * TILE_HEIGHT);
    sprite.clip_y = _offsetY + 1 + (_offsetY * TILE_WIDTH);
    sprite.clip_h = TILE_HEIGHT;
    sprite.clip_w = TILE_WIDTH;

    Position position;
    position.x = (float)(_x * TILE_WIDTH);
    position.y = (float)(_y * TILE_HEIGHT);

    tile->pos = position;
    tile->sprite = sprite;
    tile->drawn = false;

    return tile;
}

void resetMap(Map* _map) {
    resetTile(_map->root);
}

void resetTile(Tile* _tile) {
    _tile->drawn = false;

    if(_tile->up && _tile->up->drawn){
        resetTile(_tile->up);
    }
    if(_tile->right && _tile->right->drawn){
        resetTile(_tile->right);
    }
    if(_tile->down && _tile->down->drawn){
        resetTile(_tile->down);
    }
    if(_tile->left && _tile->left->drawn){
        resetTile(_tile->left);
    }
}
