#include "Systems.h"
#include "DeltaTime.h"
#include "Math.h"
#include "Input.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define PLAYER_SPEED 250.f

void velocitySystem(Position *_position, const Velocity *_velocity) {
    if(_velocity == NULL) return;

    _position->x += _velocity->vx * timer->deltaTime;
    _position->y += _velocity->vy * timer->deltaTime;
}

void drawSystem(SDL_Renderer* _renderer, const Position* _position, const Rotation* _rotation, const Sprite* _sprite, bool _debuggable) {
    if(_sprite == NULL) return;

    SDL_Rect dstRect;
    dstRect.x = (int)_position->x;
    dstRect.y = (int)_position->y;
    dstRect.w = _sprite->width;
    dstRect.h = _sprite->height;

    SDL_Rect srcRect;
    srcRect.x = _sprite->clip_x;
    srcRect.y = _sprite->clip_y;
    srcRect.w = _sprite->clip_w;
    srcRect.h = _sprite->clip_h;

    if(_rotation){
        SDL_RenderCopyEx(_renderer, _sprite->texture, &srcRect, &dstRect, _rotation->angle, NULL, SDL_FLIP_NONE);
    }
    else{
        SDL_RenderCopyEx(_renderer, _sprite->texture, &srcRect, &dstRect, 0, NULL, SDL_FLIP_NONE);
    }

#ifdef DEBUG_MODE
    if(!_debuggable) return;

    if(debugFont == NULL){
        debugFont = loadFont(_renderer, "debug.ttf", 16);
    }

    SDL_SetRenderDrawColor(_renderer, 255, 255, 0, 255);
    SDL_RenderDrawRect(_renderer, &dstRect);

    char temp[64];
    snprintf(temp, sizeof(temp),"X: %d, Y: %d\n", dstRect.x, dstRect.y);
    FC_Draw(debugFont, _renderer, (float)dstRect.x, (float)dstRect.y - 12, temp);
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
#endif
}

void collisionSystem(EntityData* _data, const Collider* _collider, const Position* _position, const Dimension* _dimension, Velocity* _velocity) {
    if(_collider->type == NO_COLLIDE || _velocity == NULL) return;

    for(uint32_t i = 0; i < _data->highestId; ++i){
        //Skip if collider is the same.
        if(&_data->collider[i] != _collider && _data->collider[i].type != NO_COLLIDE){

            Position secondPosition = _data->positions[i];
            Dimension secondDimension = _data->dimensions[i];

            Position nextPosition;
            nextPosition.x = _position->x + (_velocity->vx * timer->deltaTime);
            nextPosition.y = _position->y + (_velocity->vy * timer->deltaTime);

            if(intersectRect(&nextPosition, _dimension, &secondPosition, &secondDimension)){
                _velocity->vx = 0;
                _velocity->vy = 0;
                return;
            }
        }
    }
}

void inputSystem(EntityData* _data, const InputListener* _input, Velocity* _velocity){
    if(!_input->listens) return;

    if(input.keyPressed == SDLK_w){
        _velocity->vy = -PLAYER_SPEED;
    }

    if(input.keyPressed == SDLK_d){
        _velocity->vx = PLAYER_SPEED;
    }

    if(input.keyPressed == SDLK_s){
        _velocity->vy = PLAYER_SPEED;
    }

    if(input.keyPressed == SDLK_a){
        _velocity->vx = -PLAYER_SPEED;
    }

    if(input.keyReleased == SDLK_a || input.keyReleased == SDLK_d){
        _velocity->vx = 0;
    }

    if(input.keyReleased == SDLK_w || input.keyReleased == SDLK_s){
        _velocity->vy = 0;
    }
}