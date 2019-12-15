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

void renderMap(SDL_Renderer* _renderer, Map* _map) {
    SDL_Texture* texTarget = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, _map->width * TILE_WIDTH, _map->height * TILE_HEIGHT);

    SDL_SetRenderTarget(_renderer, texTarget);
    SDL_RenderClear(_renderer);

    renderTile(_renderer, _map->root);

    SDL_SetRenderTarget(_renderer, NULL);

    Sprite* sprite = (Sprite*)malloc(sizeof(Sprite));
    sprite->texture = texTarget;
    SDL_QueryTexture(texTarget, NULL, NULL, &sprite->width, &sprite->height);
    sprite->clip_x = 0;
    sprite->clip_y = 0;
    sprite->clip_w = sprite->width;
    sprite->clip_h = sprite->height;

    _map->mapSprite = sprite;
}

void renderTile(SDL_Renderer* _renderer, Tile* _tile) {
    SDL_Rect dstRect;
    dstRect.x = (int)_tile->pos.x;
    dstRect.y = (int)_tile->pos.y;
    dstRect.w = _tile->sprite.width;
    dstRect.h = _tile->sprite.height;

    SDL_Rect srcRect;
    srcRect.x = _tile->sprite.clip_x;
    srcRect.y = _tile->sprite.clip_y;
    srcRect.w = _tile->sprite.clip_w;
    srcRect.h = _tile->sprite.clip_h;

    SDL_RenderCopyEx(_renderer, _tile->sprite.texture, &srcRect, &dstRect, 0, NULL, SDL_FLIP_NONE);

    _tile->drawn = true;
    if(_tile->up && !_tile->up->drawn){
        renderTile(_renderer, _tile->up);
    }
    if(_tile->right && !_tile->right->drawn){
        renderTile(_renderer, _tile->right);
    }
    if(_tile->down && !_tile->down->drawn){
        renderTile(_renderer, _tile->down);
    }
    if(_tile->left && !_tile->left->drawn){
        renderTile(_renderer, _tile->left);
    }
}