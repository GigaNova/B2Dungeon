#include "BSP.h"
#include <stdlib.h>
#include <stdio.h>

Map* generateDungeon(SDL_Renderer* _renderer, uint16_t _width, uint16_t _height) {
    IntRectangle mainContainer = {0, 0, _width, _height};
    Tree* root = splitTree(0, mainContainer);
    generateRoom(root);
    generateCorridors(root);

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
    refineMap(&tileMap, _width / SCALE, _height / SCALE);
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
        if(_node->corridor.dir == HORIZONTAL){
            const uint16_t y = _node->corridor.start.y / SCALE;
            for(uint16_t x = _node->corridor.start.x / SCALE; x < _node->corridor.end.x / SCALE; ++x){
                if(x < 2 || y < 2) continue;
                Tile* newTile = createTile(_renderer, x, y, 10, 1);
                newTile->walkable = true;
                _tileMap->map[x][y] = newTile;
            }
        }
        else{
            const uint16_t x = _node->corridor.start.x / SCALE;
            for(uint16_t y = _node->corridor.start.y / SCALE; y < _node->corridor.end.y / SCALE; ++y){
                if(x < 2 || y < 2) continue;
                Tile* newTile = createTile(_renderer, x, y, 10, 1);
                newTile->walkable = true;
                _tileMap->map[x][y] = newTile;
            }
        }

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
                if(x < 2 || y < 2) continue;
                Tile* newTile = createTile(_renderer, x, y, 10, 1);
                newTile->walkable = true;
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

void refineMap(TileMap* _tileMap, uint16_t _width, uint16_t _height) {
    for(uint16_t x = 0; x < _width; ++x) {
        for(uint16_t y = 0; y < _height; ++y) {
            bool flagTop = false;
            bool flagRight = false;
            bool flagDown = false;
            bool flagLeft = false;
            bool flagTopLeft = false;
            bool flagTopRight = false;
            bool flagDownRight = false;
            bool flagDownLeft = false;

            if(x - 1 > 0){
                flagLeft = _tileMap->map[x - 1][y]->walkable;
            }
            if(x + 1 < _width){
                flagRight = _tileMap->map[x + 1][y]->walkable;
            }
            if(y - 1 > 0){
                flagTop = _tileMap->map[x][y - 1]->walkable;
            }
            if(y + 1 < _height){
                flagDown = _tileMap->map[x][y + 1]->walkable;
            }
            if(x + 1 < _width && y + 1 < _height){
                flagDownRight = _tileMap->map[x + 1][y + 1]->walkable;
            }
            if(x + 1 < _height && y - 1 > 0){
                flagTopRight = _tileMap->map[x + 1][y - 1]->walkable;
            }
            if(x - 1 > 0 && y + 1 < _height){
                flagDownLeft = _tileMap->map[x - 1][y + 1]->walkable;
            }
            if(x - 1 > 0 && y - 1 > 0){
                flagTopLeft = _tileMap->map[x - 1][y - 1]->walkable;
            }

            //Solid Wall, no walkable neighbours in any direction.
            if(!flagTop && !flagRight && !flagDown && !flagLeft &&
                !flagTopLeft && !flagTopRight && !flagDownRight && !flagDownLeft) {
                uint32_t random = randomBetween(0, 100);
                if (random < 66){
                    _tileMap->map[x][y]->sprite.clip_x = 2 + (1 * TILE_WIDTH);
                    _tileMap->map[x][y]->sprite.clip_y = 2 + (1 * TILE_HEIGHT);
                }
                else if (random < 86){
                    _tileMap->map[x][y]->sprite.clip_x = 5 + (4 * TILE_WIDTH);
                    _tileMap->map[x][y]->sprite.clip_y = 2 + (1 * TILE_HEIGHT);
                }
                else{
                    _tileMap->map[x][y]->sprite.clip_x = 8 + (7 * TILE_WIDTH);
                    _tileMap->map[x][y]->sprite.clip_y = 2 + (1 * TILE_HEIGHT);
                }
            }
            //Walkable neighbour to the right.
            else if(!flagTop && flagRight && !flagDown && !flagLeft && !_tileMap->map[x][y]->walkable) {
                uint32_t random = randomBetween(0, 100);
                if (random < 33){
                    _tileMap->map[x][y]->sprite.clip_x = 3 + (2 * TILE_WIDTH);
                    _tileMap->map[x][y]->sprite.clip_y = 2 + (1 * TILE_HEIGHT);
                }
                else if (random < 66){
                    _tileMap->map[x][y]->sprite.clip_x = 6 + (5 * TILE_WIDTH);
                    _tileMap->map[x][y]->sprite.clip_y = 2 + (1 * TILE_HEIGHT);
                }
                else{
                    _tileMap->map[x][y]->sprite.clip_x = 9 + (8 * TILE_WIDTH);
                    _tileMap->map[x][y]->sprite.clip_y = 2 + (1 * TILE_HEIGHT);
                }
            }
            //Walkable neighbour to the left.
            else if(!flagTop && !flagRight && !flagDown && flagLeft && !_tileMap->map[x][y]->walkable) {
                uint32_t random = randomBetween(0, 100);
                if (random < 33){
                    _tileMap->map[x][y]->sprite.clip_x = 1;
                    _tileMap->map[x][y]->sprite.clip_y = 2 + (1 * TILE_HEIGHT);
                }
                else if (random < 66){
                    _tileMap->map[x][y]->sprite.clip_x = 4 + (3 * TILE_WIDTH);
                    _tileMap->map[x][y]->sprite.clip_y = 2 + (1 * TILE_HEIGHT);
                }
                else{
                    _tileMap->map[x][y]->sprite.clip_x = 7 + (6 * TILE_WIDTH);
                    _tileMap->map[x][y]->sprite.clip_y = 2 + (1 * TILE_HEIGHT);
                }
            }
            //Walkable neighbour down.
            else if(!flagTop && !flagRight && flagDown && !flagLeft && !_tileMap->map[x][y]->walkable) {
                uint32_t random = randomBetween(0, 100);
                if (random < 33){
                    _tileMap->map[x][y]->sprite.clip_x = 2 + (1 * TILE_WIDTH);
                    _tileMap->map[x][y]->sprite.clip_y = 3 + (2 * TILE_HEIGHT);
                }
                else if (random < 66){
                    _tileMap->map[x][y]->sprite.clip_x = 5 + (4 * TILE_WIDTH);
                    _tileMap->map[x][y]->sprite.clip_y = 3 + (2 * TILE_HEIGHT);
                }
                else{
                    _tileMap->map[x][y]->sprite.clip_x = 8 + (7 * TILE_WIDTH);
                    _tileMap->map[x][y]->sprite.clip_y = 3 + (2 * TILE_HEIGHT);
                }
            }
            //Walkable neighbour to the top.
            else if(flagTop && !flagRight && !flagDown && !flagLeft && !_tileMap->map[x][y]->walkable) {
                uint32_t random = randomBetween(0, 100);
                if (random < 33){
                    _tileMap->map[x][y]->sprite.clip_x = 2 + (1 * TILE_WIDTH);
                    _tileMap->map[x][y]->sprite.clip_y = 1;
                }
                else if (random < 66){
                    _tileMap->map[x][y]->sprite.clip_x = 5 + (4 * TILE_WIDTH);
                    _tileMap->map[x][y]->sprite.clip_y = 1;
                }
                else{
                    _tileMap->map[x][y]->sprite.clip_x = 8 + (7 * TILE_WIDTH);
                    _tileMap->map[x][y]->sprite.clip_y = 1;
                }
            }
            //Corner left-down
            else if(!flagTop && !flagRight && flagLeft && flagDown && flagDownLeft && !_tileMap->map[x][y]->walkable){
                _tileMap->map[x][y]->sprite.clip_x = 1;
                _tileMap->map[x][y]->sprite.clip_y = 3 + (2 * TILE_HEIGHT);
            }
            //Corner right-down
            else if(!flagTop && !flagLeft && flagRight && flagDown && flagDownRight && !_tileMap->map[x][y]->walkable){
                _tileMap->map[x][y]->sprite.clip_x = 3 + (2 * TILE_WIDTH);
                _tileMap->map[x][y]->sprite.clip_y = 3 + (2 * TILE_HEIGHT);
            }
            //Corner left-up
            else if(!flagDown && !flagRight && flagLeft && flagTop && flagTopLeft && !_tileMap->map[x][y]->walkable){
                _tileMap->map[x][y]->sprite.clip_x = 1;
                _tileMap->map[x][y]->sprite.clip_y = 1;
            }
            //Corner right-up
            else if(!flagDown && !flagLeft && flagRight && flagTop && flagTopRight && !_tileMap->map[x][y]->walkable){
                _tileMap->map[x][y]->sprite.clip_x = 3 + (2 * TILE_WIDTH);
                _tileMap->map[x][y]->sprite.clip_y = 1;
            }
            //Thin wall vertical
            else if(!flagTop && flagRight && !flagDown && flagLeft && !_tileMap->map[x][y]->walkable) {
                _tileMap->map[x][y]->sprite.clip_x = 1;
                _tileMap->map[x][y]->sprite.clip_y = 5 + (4 * TILE_HEIGHT);
            }
            //Thin wall horizontal
            else if(flagTop && !flagRight && flagDown && !flagLeft && !_tileMap->map[x][y]->walkable) {
                _tileMap->map[x][y]->sprite.clip_x = 2 + (1 * TILE_WIDTH);
                _tileMap->map[x][y]->sprite.clip_y = 4 + (3 * TILE_HEIGHT);
            }
            //Pointy thing right
            else if(flagTop && flagRight && flagDown && !flagLeft && !_tileMap->map[x][y]->walkable) {
                _tileMap->map[x][y]->sprite.clip_x = 3 + (2 * TILE_WIDTH);
                _tileMap->map[x][y]->sprite.clip_y = 8 + (7 * TILE_HEIGHT);
            }
            //Pointy thing left
            else if(flagTop && !flagRight && flagDown && flagLeft && !_tileMap->map[x][y]->walkable) {
                _tileMap->map[x][y]->sprite.clip_x = 1;
                _tileMap->map[x][y]->sprite.clip_y = 8 + (7 * TILE_HEIGHT);
            }
            //Pointy thing up
            else if(flagTop && flagRight && !flagDown && flagLeft && !_tileMap->map[x][y]->walkable) {
                _tileMap->map[x][y]->sprite.clip_x = 2 + (1 * TILE_WIDTH);
                _tileMap->map[x][y]->sprite.clip_y = 7 + (6 * TILE_HEIGHT);
            }
            //Pointy thing down
            else if(!flagTop && flagRight && flagDown && flagLeft && !_tileMap->map[x][y]->walkable) {
                _tileMap->map[x][y]->sprite.clip_x = 2 + (1 * TILE_WIDTH);
                _tileMap->map[x][y]->sprite.clip_y = 9 + (8 * TILE_HEIGHT);
            }
            //Rock
            else if(flagTop && flagRight && flagDown && flagLeft && !_tileMap->map[x][y]->walkable) {
                _tileMap->map[x][y]->sprite.clip_x = 2 + (1 * TILE_WIDTH);
                _tileMap->map[x][y]->sprite.clip_y = 5 + (4 * TILE_HEIGHT);
            }
            //Walkable ground
            else if(flagTop || flagRight || flagDown || flagLeft) {
                uint32_t random = randomBetween(0, 100);
                if (random < 80){
                    _tileMap->map[x][y]->sprite.clip_x = 11 + (10 * TILE_WIDTH);
                    _tileMap->map[x][y]->sprite.clip_y = 2 + (1 * TILE_HEIGHT);
                }
                else if (random < 90){
                    _tileMap->map[x][y]->sprite.clip_x = 11 + (10 * TILE_WIDTH);
                    _tileMap->map[x][y]->sprite.clip_y = 4 + (3 * TILE_HEIGHT);
                }
                else{
                    _tileMap->map[x][y]->sprite.clip_x = 11 + (10 * TILE_WIDTH);
                    _tileMap->map[x][y]->sprite.clip_y = 5 + (4 * TILE_HEIGHT);
                }
            }
            else if(flagDownRight) {
                _tileMap->map[x][y]->sprite.clip_x = 1;
                _tileMap->map[x][y]->sprite.clip_y = 16 + (15 * TILE_HEIGHT);
            }
            else if(flagTopRight) {
                _tileMap->map[x][y]->sprite.clip_x = 1;
                _tileMap->map[x][y]->sprite.clip_y = 17 + (16 * TILE_HEIGHT);
            }
            else if(flagDownLeft) {
                _tileMap->map[x][y]->sprite.clip_x = 2 + (1 * TILE_WIDTH);
                _tileMap->map[x][y]->sprite.clip_y = 16 + (15 * TILE_HEIGHT);
            }
            else if(flagTopLeft) {
                _tileMap->map[x][y]->sprite.clip_x = 2 + (1 * TILE_WIDTH);
                _tileMap->map[x][y]->sprite.clip_y = 17 + (16 * TILE_HEIGHT);
            }
        }
    }
}

void generateCorridors(Tree* _node) {
    if(!_node->rightChild || !_node->leftChild){
        return;
    }

    Point centerA = getCenter(&_node->leftChild->container);
    Point centerB = getCenter(&_node->rightChild->container);

    if(centerA.y != centerB.y){
        _node->corridor.dir = VERTICAL;
        _node->corridor.start = centerA.y < centerB.y ? centerA : centerB;
        _node->corridor.end = centerA.y < centerB.y ? centerB : centerA;
    }else{
        _node->corridor.dir = HORIZONTAL;
        _node->corridor.start = centerA.x < centerB.x ? centerA : centerB;
        _node->corridor.end = centerA.x < centerB.x ? centerB : centerA;
    }

    generateCorridors(_node->leftChild);
    generateCorridors(_node->rightChild);
}
