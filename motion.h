#ifndef MOTION_H
#define MOTION_H

#include "box.h"

enum {
    SCREEN_WIDTH = 640,
    SCREEN_HEIGHT = 480,
    SCREEN_BPP = 32,
    FPS = 60,

    PADDLE_SPEED_LIMIT = 16,
    BALL_SPEED_LIMIT = 4,
    BALL_LIFE_FROM = 5,
    BRICKGRID_HSIZE = 14,
    BRICKGRID_VSIZE = 8,

    FIELD_LEFT = 16,
    FIELD_TOP = 16,
    FIELD_RIGHT = 438,
    FIELD_BOTTOM = 464,

    PADDLE_LEFT = 203,
    PADDLE_TOP = 428,
    PADDLE_RIGHT = 251,
    PADDLE_BOTTOM = 432,

    BALL_LEFT = 223,
    BALL_TOP = 420,
    BALL_RIGHT = 231,
    BALL_BOTTOM = 428,

    BRICK_LEFT = 18,
    BRICK_TOP = 48,
    BRICK_RIGHT = 46,
    BRICK_BOTTOM = 56,

    BRICK_GAP = 2,

    SCORE_H = 460, SCORE_V = 80,
    LIVES_H = 460, LIVES_V = 420
};

typedef struct motion_s {
    box_t* field;
    box_t* paddle;
    box_t* ball;
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
} motion_t;

void init_motion (motion_t* const game);
void step_motion (motion_t* const game, int const paddle_speed, int const releaseball);

static inline box_t*
ref_brick (motion_t* const game, int h, int v)
{
    return game->brickgrid + v * game->grid_hsize + h;
}

#endif

