#ifndef POKEMONMYSTERYCREATOR_MAP_H
#define POKEMONMYSTERYCREATOR_MAP_H

#include "Entity.h"

#define TILE_WIDTH 24
#define TILE_HEIGHT 24

//Forward declaration
struct Tile{
    struct Tile* up;
    struct Tile* right;
    struct Tile* down;
    struct Tile* left;

    Position pos;
    Sprite sprite;

    bool drawn;
};
typedef struct Tile Tile;

typedef struct{
    Tile* root;
    uint16_t width;
    uint16_t height;
} Map;

void insertTileX(Tile* _tile, Tile* _toBeInserted);
void insertTileY(Tile* _tile, Tile* _toBeInserted);

Tile* createTile(SDL_Renderer* _renderer, uint16_t _x, uint16_t _y, uint16_t _offsetX, uint16_t _offsetY);
Map* createMap(SDL_Renderer* _renderer, uint16_t _width, uint16_t _height);

void resetMap(Map* _map);
void resetTile(Tile* _tile);

#endif //POKEMONMYSTERYCREATOR_MAP_H
