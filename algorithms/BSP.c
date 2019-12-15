#include "BSP.h"
#include <stdlib.h>
#include <stdio.h>

Map* generateDungeon(SDL_Renderer* _renderer, uint16_t _width, uint16_t _height) {
    IntRectangle mainContainer = {0, 0, _width, _height};
    Tree* root = splitTree(0, mainContainer);
    generateRoom(root);

    return convertToMap(_renderer, root, _width, _height);
}

Map* convertToMap(SDL_Renderer* _renderer, Tree* _root, uint16_t _width, uint16_t _height) {
    Map* map = (Map*)malloc(sizeof(Map));

    TileMap tileMap;
    for(uint16_t x = 0; x < MAX_MAP_SIZE; ++x){
        for(uint16_t y = 0; y < MAX_MAP_SIZE; ++y){
            tileMap.map[x][y] = createTile(_renderer, x, y, 1, 1);
        }
    }

    convertNode(_renderer, _root, &tileMap);
    linkNodes(&tileMap, _width / SCALE, _height / SCALE);

    map->root = tileMap.map[0][0];
    map->width = _width / SCALE;
    map->height = _height / SCALE;

    destroyTree(_root);

    return map;
}

void linkNodes(TileMap* _tileMap, uint16_t _width, uint16_t _height) {
    for(uint16_t x = 0; x < _width; ++x) {
        for (uint16_t y = 0; y < _height; ++y) {
            Tile* tile = _tileMap->map[x][y];
            if(x - 1 >= 0){
                tile->left = _tileMap->map[x - 1][y];
            }
            if(x + 1 < _width){
                tile->right = _tileMap->map[x + 1][y];
            }
            if(y - 1 >= 0){
                tile->up = _tileMap->map[x][y - 1];
            }
            if(y + 1 < _height){
                tile->down = _tileMap->map[x][y + 1];
            }
        }
    }
}

void convertNode(SDL_Renderer* _renderer, Tree* _node, TileMap* _tileMap) {
    if(!isLeaf(_node)){
        if(_node->rightChild){
            convertNode(_renderer, _node->rightChild, _tileMap);
        }

        if(_node->leftChild){
            convertNode(_renderer, _node->leftChild, _tileMap);
        }
    }
    else{
        for(uint16_t x = _node->room.x / SCALE; x < (uint16_t)(_node->room.x / SCALE) + (uint16_t)(_node->room.w / SCALE); ++x){
            for(uint16_t y = _node->room.y / SCALE; y < (uint16_t)(_node->room.y / SCALE) + (uint16_t)(_node->room.h / SCALE); ++y){
                if(x == 0 || y == 0) continue;
                Tile* newTile = createTile(_renderer, x, y, 10, 1);
                _tileMap->map[x][y] = newTile;
            }
        }
    }
}

Tree* splitTree(uint8_t _depth, IntRectangle _container) {
    Tree* node = (Tree*)malloc(sizeof(Tree));
    node->container = _container;
    node->leftChild = NULL;
    node->rightChild = NULL;

    if(_depth == MAX_DEPTH){
        return node;
    }

    SplitRectangle split = splitContainer(_container);
    node->leftChild = splitTree(_depth + 1, split.left);
    node->rightChild = splitTree(_depth + 1, split.right);

    return node;
}

void destroyTree(Tree* _node) {
    if(_node->rightChild){
        destroyTree(_node->rightChild);
    }

    if(_node->leftChild){
        destroyTree(_node->leftChild);
    }

    free(_node);
}

SplitRectangle splitContainer(IntRectangle _container) {
    IntRectangle left;
    IntRectangle right;
    SplitRectangle split;

    if(randomBetween(0, 10) > 5){
        left.x = _container.x;
        left.y = _container.y;
        left.w = _container.w;
        left.h = randomBetween((int32_t)_container.h * 0.3f, (int32_t)_container.h * 0.5);

        right.x = _container.x;
        right.y = _container.y + left.h;
        right.w = _container.w;
        right.h = _container.h - left.h;

        float lhsHeightRatio = (float)left.h / (float)left.w;
        float rhsHeightRatio = (float)right.h / (float)right.w;
        if(lhsHeightRatio < HEIGHT_RATIO || rhsHeightRatio < HEIGHT_RATIO){
            return splitContainer(_container);
        }
    }
    else{
        left.x = _container.x;
        left.y = _container.y;
        left.w = randomBetween((int32_t)_container.w * 0.3f, (int32_t)_container.w * 0.5);
        left.h = _container.h;

        right.x = _container.x + left.w;
        right.y = _container.y;
        right.w = _container.w - left.w;
        right.h = _container.h;

        float lhsWidthRatio = (float)left.w / (float)left.h;
        float rhsWidthRatio = (float)right.w / (float)right.h;
        if(lhsWidthRatio < WIDTH_RATIO || rhsWidthRatio < WIDTH_RATIO){
            return splitContainer(_container);
        }
    }

    split.left = left;
    split.right = right;
    return split;
}

bool isLeaf(Tree* _node) {
    return !_node->rightChild && !_node->leftChild;
}

void debugBSPDraw(SDL_Renderer* _renderer, Tree* _node) {
    if(isLeaf(_node)){

        SDL_Rect boundRect;
        boundRect.x = _node->container.x;
        boundRect.y = _node->container.y;
        boundRect.w = _node->container.w;
        boundRect.h = _node->container.h;

        SDL_Rect roomRect;
        roomRect.x = _node->room.x;
        roomRect.y = _node->room.y;
        roomRect.w = _node->room.w;
        roomRect.h = _node->room.h;

        SDL_SetRenderDrawColor(_renderer, 0, 255, 0, 255);
        SDL_RenderDrawRect(_renderer, &boundRect);
        SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(_renderer, &roomRect);
        SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
        return;
    }

    if(_node->rightChild){
        debugBSPDraw(_renderer, _node->rightChild);
    }

    if(_node->leftChild){
        debugBSPDraw(_renderer, _node->leftChild);
    }
}

void generateRoom(Tree* _node) {
    if(!isLeaf(_node)){
        if(_node->leftChild){
            generateRoom(_node->leftChild);
        }

        if(_node->rightChild){
            generateRoom(_node->rightChild);
        }
    }
    else{
        int32_t x = randomBetween(MIN_ROOM_DISTANCE, _node->container.w / 4);
        int32_t y = randomBetween(MIN_ROOM_DISTANCE, _node->container.h / 4);

        IntRectangle room = {
                _node->container.x + x,
                _node->container.y + y,
                _node->container.w - (int16_t)((float)x * (randomBetween(20, 30) / 10.f)),
                _node->container.h - (int16_t)((float)y * (randomBetween(20, 30) / 10.f)),
        };

        _node->room = room;
    }
}

Point getCenter(IntRectangle* _rect) {
    Point result;

    result.x = _rect->x + (_rect->w / 2);
    result.y = _rect->y + (_rect->h / 2);

    return result;
}
