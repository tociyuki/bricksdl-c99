#include "motion.h"
#include "box.h"
#include <stddef.h>
#include <stdbool.h>

static void start_motion (motion_t* const game);
static void continue_motion (motion_t* const game);
static void ball_motion (motion_t* const game,
    int const paddle_speed, int const releaseball);
static void slide_paddle (motion_t* const game,
    box_t* const paddle0, int const speed0);
static bool hit_paddle (motion_t* const game, box_t* const paddle0);
static bool hit_brick (motion_t* const game);
static bool bounce_field (motion_t* const game);
static bool vhit_box (box_t* const field, box_t* const a, box_t* const ball);
static bool hhit_box (box_t* const field, box_t* const a, box_t* const ball);

void
init_motion (motion_t* const game)
{
    box_t* const brk0 = ref_brick (game, 0, 0);
    int const brick_width = brk0->right - brk0->left;
    int const brick_height = brk0->bottom - brk0->top;
    for (int j = 0; j < game->grid_vsize; j++) {
        for (int i = 0; i < game->grid_hsize; i++) {
            box_t* const brick = ref_brick (game, i, j);
            int const left = brk0->left + (brick_width + game->brick_gap) * i;
            int const top = brk0->top + (brick_height + game->brick_gap) * j;
            set_box (left, top, left + brick_width, top + brick_height,
                0, 0, brick);
            brick->alive = true;
        }
    }
    start_motion (game);
}

void
step_motion (motion_t* const game, int const paddle_speed, int const releaseball)
{
    if (game->ball_life > 0 && game->brick_count > 0)
        ball_motion (game, paddle_speed, releaseball);
    else if (releaseball)
        start_motion (game);
}

static void
start_motion (motion_t* const game)
{
    game->score = 0;
    game->ball_life = game->ball_life_from;
    game->brick_count = game->grid_vsize * game->grid_hsize;
    for (int j = 0; j < game->grid_vsize; j++)
        for (int i = 0; i < game->grid_hsize; i++)
            ref_brick(game, i, j)->alive = true;
    continue_motion (game);
}

static void
continue_motion (motion_t* const game)
{
    game->ball_cought = true;
    int const center = box_hcenter (game->field);
    int const paddle_width = game->paddle->right - game->paddle->left;
    int const ball_width = game->ball->right - game->ball->left;
    int const ball_height = game->ball->bottom - game->ball->top;
    set_box (center - paddle_width / 2, game->paddle->top,
        center + paddle_width / 2, game->paddle->bottom,
        0, 0, game->paddle);
    set_box (center - ball_width / 2, game->paddle->top - ball_height,
        center + ball_width / 2, game->paddle->top,
        0, 0, game->ball);
}

static void
ball_motion (motion_t* const game, int const paddle_speed, int const releaseball)
{
    box_t paddle0;
    box_t* const field = game->field;
    box_t* const paddle = game->paddle;
    box_t* const ball = game->ball;
    slide_paddle (game, &paddle0, paddle_speed);
    if (game->ball_cought) {
        int const h0 = box_hcenter (ball);
        int const h1 = box_hcenter (paddle);
        offset_box (ball, h1 - h0, 0, ball);
        if (releaseball) {
            game->ball_cought = false;
            set_boxspeed (paddle->hspeed / 4, -4, ball);
        }
    }
    else {
        if (! hit_paddle (game, &paddle0)) {
            if (! hit_brick (game))
                bounce_field (game);
        }
        if (ball->bottom == field->bottom) {
            if (--game->ball_life > 0)
                continue_motion (game);
        }
    }
}

static void
slide_paddle (motion_t* const game, box_t* const paddle0, int const speed0)
{
    box_t* const paddle = game->paddle;
    box_t* const field = game->field;
    int const limit = game->paddle_speed_limit;
    int const speed = speed0 > limit ? limit : speed0 < -limit ? -limit : speed0;
    set_boxspeed (speed, 0, paddle);
    offset_box (paddle, 0, 0, paddle0);
    offset_box (paddle, speed, 0, paddle);
    clip_box (field, paddle);
    if (field->left >= paddle->left || field->right <= paddle->right)
        set_boxspeed (0, 0, paddle);
}

static bool
hit_paddle (motion_t* const game, box_t* const paddle0)
{
    box_t* const ball = game->ball;
    box_t* const paddle = game->paddle;
    if (ball->vspeed > 0 && vhit_box (game->field, paddle0, ball)) {
        int const x = ball->hspeed + paddle->hspeed;
        int const limit = game->ball_speed_limit;
        int const speed = x > limit ? limit : x < -limit ? -limit : x;
        set_boxspeed (speed, -ball->vspeed, ball);
        return true;
    }
    return false;
}

static bool
hit_brick (motion_t* const game)
{
    box_t* const ball = game->ball;
    int const hsize = game->grid_hsize;
    int const vsize = game->grid_vsize;
    int const i0 = ball->hspeed < 0 ? hsize - 1 : 0;
    int const j0 = ball->vspeed < 0 ? vsize - 1 : 0;
    int const di = i0 == 0 ? +1 : -1;
    int const dj = j0 == 0 ? +1 : -1;
    for (int i = i0; i >=0 && i < hsize; i += di) {
        for (int j = j0; j >=0 && j < vsize; j += dj) {
            box_t* const brick = ref_brick (game, i, j);
            if (! brick->alive)
                continue;
            bool hit = true;
            if (vhit_box (game->field, brick, ball))
                set_boxspeed (ball->hspeed, -ball->vspeed, ball);
            else if (ball->hspeed != 0 && hhit_box (game->field, brick, ball))
                set_boxspeed (-ball->hspeed, ball->vspeed, ball);
            else
                hit = false;
            if (hit) {
                brick->alive = false;
                game->score += 100;
                game->brick_count--;
                return true;
            }
        }
    }
    return false;
}

static bool
bounce_field (motion_t* const game)
{
    box_t* const ball = game->ball;
    box_t* const field = game->field;
    offset_box (ball, ball->hspeed, ball->vspeed, ball);
    clip_box (field, ball);
    if (field->left >= ball->left || field->right <= ball->right)
        set_boxspeed (-ball->hspeed, ball->vspeed, ball);
    if (field->top >= ball->top || field->bottom <= ball->bottom)
        set_boxspeed (ball->hspeed, -ball->vspeed, ball);
    return true;
}

static inline int int_abs (int a)
{
    return a >= 0 ? a : -a;
}

/* hit top or bottom side of whether the paddle or the brick */
static bool
vhit_box (box_t* const field, box_t* const a, box_t* const ball)
{
    box_t a1;
    box_t ball1;

    int const dv = ball->vspeed < 0 ? a->bottom - ball->top : a->top - ball->bottom;
    offset_box (ball, dv * ball->hspeed / ball->vspeed, dv, &ball1);
    offset_box (a, dv * a->hspeed / ball->vspeed, 0, &a1);
    if (a->hspeed != 0)
        clip_box (field, &a1);
    if (((ball->vspeed < 0 && dv <= 0) || (ball->vspeed > 0 && dv >= 0))
            && int_abs (dv) <= int_abs (ball->vspeed)
            && field->left <= ball1.left && ball1.right <= field->right
            && sect_box (&a1, &ball1, NULL)) {
        offset_box (&ball1, 0, 0, ball);
        return true;
    }
    return false;
}

/* hit left or right side of the brick */
static bool
hhit_box (box_t* const field, box_t* const a, box_t* const ball)
{
    box_t ball1;

    int const dh = ball->hspeed < 0 ? a->right - ball->left : a->left - ball->right;
    offset_box (ball, dh, dh * ball->vspeed / ball->hspeed, &ball1);
    if (((ball->hspeed < 0 && dh <= 0) || (ball->hspeed > 0 && dh >= 0))
            && int_abs (dh) <= int_abs (ball->hspeed)
            && field->top <= ball1.top && ball1.bottom <= field->bottom
            && sect_box (a, &ball1, NULL)) {
        offset_box (&ball1, 0, 0, ball);
        return true;
    }
    return false;
}

