#ifndef POKEMONMYSTERYCREATOR_BSP_H
#define POKEMONMYSTERYCREATOR_BSP_H

#include <stdint.h>
#include "../Math.h"

#define MIN_ROOM_DISTANCE 2
#define MAX_DEPTH 5

typedef struct{
    int16_t x;
    int16_t y;
    uint16_t w;
    uint16_t h;
} IntRectangle;

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

Tree* generateDungeon(uint16_t _width, uint16_t _height);

Tree* splitTree(uint8_t _depth, IntRectangle _container);
SplitRectangle splitContainer(IntRectangle _container);
void generateRoom(Tree* _node);

bool isLeaf(Tree* _node);

void debugBSPDraw(SDL_Renderer* _renderer, Tree* _node);
void destroyTree(Tree* _node);
#endif //POKEMONMYSTERYCREATOR_BSP_H
