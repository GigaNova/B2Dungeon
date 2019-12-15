#ifndef POKEMONMYSTERYCREATOR_BSP_H
#define POKEMONMYSTERYCREATOR_BSP_H

#include <stdint.h>
#include "../Math.h"
#include "../Map.h"

#define MIN_ROOM_DISTANCE 2
#define MAX_DEPTH 5
#define MAX_MAP_SIZE 512
#define SCALE 28

#define WIDTH_RATIO 0.45
#define HEIGHT_RATIO 0.45

typedef struct{
    int16_t x;
    int16_t y;
    uint16_t w;
    uint16_t h;
} IntRectangle;

typedef struct {
    int16_t x;
    int16_t y;
} Point;

typedef struct {
    IntRectangle left;
    IntRectangle right;
} SplitRectangle;

struct Tree{
    IntRectangle container;
    IntRectangle room;
    struct Tree* leftChild;
    struct Tree* rightChild;
};
typedef struct Tree Tree;

typedef struct {
    Tile* map[MAX_MAP_SIZE][MAX_MAP_SIZE];
} TileMap;

Map* generateDungeon(SDL_Renderer* _renderer, uint16_t _width, uint16_t _height);

Map* convertToMap(SDL_Renderer* _renderer, Tree* _root, uint16_t _width, uint16_t _height);
void convertNode(SDL_Renderer* _renderer, Tree* _node, TileMap* _tileMap);
void linkNodes(TileMap* _tileMap, uint16_t _width, uint16_t _height);

Tree* splitTree(uint8_t _depth, IntRectangle _container);
SplitRectangle splitContainer(IntRectangle _container);

void generateRoom(Tree* _node);
void generateCorridors(Tree* _node);

bool isLeaf(Tree* _node);
Point getCenter(IntRectangle* _rect);

void refineMap(TileMap* _tileMap, uint16_t _width, uint16_t _height);

void debugBSPDraw(SDL_Renderer* _renderer, Tree* _node);
void destroyTree(Tree* _node);
#endif //POKEMONMYSTERYCREATOR_BSP_H
