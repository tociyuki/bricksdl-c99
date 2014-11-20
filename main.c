#include "box.h"
#include "motion.h"
#include "viewsdl.h"
#include "SDL/SDL.h"
#include "SDL/SDL_gfxPrimitives.h"
#include <stdlib.h>
#include <stdbool.h>

enum {
    CMD_QUIT = 1,
    CMD_RELEASEBALL = 2
};

static int
game_mainloop ()
{
    motion_t game;
    box_t field;
    box_t paddle;
    box_t ball;
    box_t brickgrid[BRICKGRID_HSIZE * BRICKGRID_VSIZE];

    game.field = &field;
    game.paddle = &paddle;
    game.ball = &ball;
    game.brickgrid = brickgrid;
    init_motion (&game);

    if (SDL_Init (SDL_INIT_EVERYTHING) == -1)
        return EXIT_FAILURE;
    SDL_Surface* screen = SDL_SetVideoMode (SCREEN_WIDTH, SCREEN_HEIGHT,
        SCREEN_BPP, SDL_SWSURFACE);
    if (screen == NULL)
        return EXIT_FAILURE;
    SDL_WM_SetCaption ("bricksdl - breakout with Simple DirectMedia Layer", NULL);

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

