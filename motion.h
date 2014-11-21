#ifndef MOTION_H
#define MOTION_H

#include "box.h"

typedef struct motion_s {
    box_t* field;
    box_t* paddle;
    box_t* ball;
    box_t* scorebox;
    box_t* lifebox;
    box_t* brickgrid;
    int paddle_speed_limit;
    int ball_speed_limit;
    int ball_cought;
    int ball_life_from;
    int ball_life;
    int score;
    int brick_count;
    int grid_hsize;
    int grid_vsize;
    int brick_gap;
} motion_t;

void init_motion (motion_t* const game);
void step_motion (motion_t* const game, int const paddle_speed, int const releaseball);

static inline box_t*
ref_brick (motion_t* const game, int h, int v)
{
    return game->brickgrid + v * game->grid_hsize + h;
}

#endif

