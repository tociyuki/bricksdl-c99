#include "box.h"
#include "motion.h"
#include "viewsdl.h"
#include "SDL/SDL.h"
#include <stdlib.h>
#include <stdbool.h>

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
    BRICK_GAP = 1,

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

    SCORE_LEFT = 460,
    SCORE_TOP = 80,
    SCORE_RIGHT = 588,
    SCORE_BOTTOM = 104,

    LIFE_LEFT = 460,
    LIFE_TOP = 420,
    LIFE_RIGHT = 588,
    LIFE_BOTTOM = 444,

    BRICK_LEFT = 18,
    BRICK_TOP = 48,
    BRICK_RIGHT = 46,
    BRICK_BOTTOM = 56,
};

static void
game_setup (motion_t* const game)
{
    game->paddle_speed_limit = PADDLE_SPEED_LIMIT;
    game->ball_speed_limit = BALL_SPEED_LIMIT;
    game->ball_life_from = BALL_LIFE_FROM;
    game->brick_gap = BRICK_GAP;

    set_box (FIELD_LEFT, FIELD_TOP, FIELD_RIGHT, FIELD_BOTTOM, 0, 0, game->field);
    set_box (PADDLE_LEFT, PADDLE_TOP, PADDLE_RIGHT, PADDLE_BOTTOM, 0, 0, game->paddle);
    set_box (BALL_LEFT, BALL_TOP, BALL_RIGHT, BALL_BOTTOM, 0, 0, game->ball);
    set_box (BRICK_LEFT, BRICK_TOP, BRICK_RIGHT, BRICK_BOTTOM, 0, 0, game->brickgrid);
    set_box (SCORE_LEFT, SCORE_TOP, SCORE_RIGHT, SCORE_BOTTOM, 0, 0, game->scorebox);
    set_box (LIFE_LEFT, LIFE_TOP, LIFE_RIGHT, LIFE_BOTTOM, 0, 0, game->lifebox);

    init_motion (game);
}

static int
game_mainloop ()
{
    enum {CMD_QUIT = 1, CMD_RELEASEBALL = 2};

    motion_t game;
    box_t field;
    box_t paddle;
    box_t ball;
    box_t scorebox;
    box_t lifebox;
    box_t brickgrid[BRICKGRID_HSIZE * BRICKGRID_VSIZE];

    game.field = &field;
    game.paddle = &paddle;
    game.ball = &ball;
    game.scorebox = &scorebox;
    game.lifebox = &lifebox;
    game.brickgrid = brickgrid;
    game.grid_hsize = BRICKGRID_HSIZE;
    game.grid_vsize = BRICKGRID_VSIZE;
    game_setup (&game);

    if (SDL_Init (SDL_INIT_EVERYTHING) == -1)
        return EXIT_FAILURE;
    SDL_Surface* const screen = SDL_SetVideoMode (SCREEN_WIDTH, SCREEN_HEIGHT,
        SCREEN_BPP, SDL_SWSURFACE);
    if (screen == NULL)
        return EXIT_FAILURE;
    SDL_WM_SetCaption ("bricksdl on Simple DirectMedia Layer", NULL);

    int cmd = 0;
    while (! (cmd & CMD_QUIT)) {
        SDL_Event event;
        int mouse_x;
        int mouse_y;

        unsigned int const start_tick = SDL_GetTicks ();
        SDL_GetMouseState (&mouse_x, &mouse_y);
        cmd = 0;
        while (SDL_PollEvent (&event)) {
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == 1)
                    cmd |= CMD_RELEASEBALL;
            }
            if (event.type == SDL_QUIT)
                cmd |= CMD_QUIT;
        }
        int const paddle_speed = mouse_x - box_hcenter (&paddle);
        step_motion (&game, paddle_speed, cmd & CMD_RELEASEBALL);
        draw_frame (screen, &game);
        if (SDL_Flip (screen) == -1)
            break;
        unsigned int const end_tick = SDL_GetTicks ();
        unsigned int const delta_tick = end_tick - start_tick;
        if (delta_tick < (1000 / FPS))
            SDL_Delay ((1000 / FPS) - delta_tick);
    }

    SDL_Quit ();
    return EXIT_SUCCESS;
}

int
main (int argc, char* argv[])
{
    return game_mainloop ();
}

