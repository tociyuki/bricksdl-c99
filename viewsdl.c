#include "box.h"
#include "motion.h"
#include "viewsdl.h"
#include "SDL/SDL.h"

enum { FONT_WIDTH = 5, FONT_HEIGHT = 7 };

static char const font7x5[10][8] = {
    {0x0e, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0e, 0}, /* 0 */
    {0x04, 0x0c, 0x04, 0x04, 0x04, 0x04, 0x0e, 0}, /* 1 */
    {0x0e, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1f, 0}, /* 2 */
    {0x1f, 0x02, 0x04, 0x02, 0x01, 0x11, 0x0e, 0}, /* 3 */
    {0x02, 0x06, 0x0a, 0x12, 0x1f, 0x02, 0x02, 0}, /* 4 */
    {0x1f, 0x10, 0x1e, 0x01, 0x01, 0x11, 0x0e, 0}, /* 5 */
    {0x06, 0x08, 0x10, 0x1e, 0x11, 0x11, 0x0e, 0}, /* 6 */
    {0x1f, 0x01, 0x02, 0x04, 0x04, 0x04, 0x04, 0}, /* 7 */
    {0x0e, 0x11, 0x11, 0x0e, 0x11, 0x11, 0x0e, 0}, /* 8 */
    {0x0e, 0x11, 0x11, 0x0f, 0x01, 0x02, 0x0c, 0}  /* 9 */
};

static void draw_field (SDL_Surface* const screen, motion_t* const game);
static void draw_ball (SDL_Surface* const screen, motion_t* const game);
static void draw_paddle (SDL_Surface* const screen, motion_t* const game);
static void draw_bricks (SDL_Surface* const screen, motion_t* const game);
static void draw_decimal (SDL_Surface* const screen, int const x, int const y, int n);
static void draw_digit (SDL_Surface* const screen, int const x, int const y, char ch);
static void draw_fillbox (SDL_Surface* const screen,
    int left, int top, int right, int bottom, int r, int g, int b);
static void draw_fillcircle (SDL_Surface* const screen,
    int x0, int y0, int radius, int r, int g, int b);

void
draw_frame (SDL_Surface* const screen, motion_t* const game)
{
    SDL_FillRect (screen, NULL, SDL_MapRGB(screen->format, 217, 217, 217));
    draw_field (screen, game);
    draw_paddle (screen, game);
    draw_ball (screen, game);
    draw_bricks (screen, game);
    draw_decimal (screen, game->scorebox->left, game->scorebox->top, game->score);
    int const n = game->ball_life > 0 ? game->ball_life - 1 : 0;
    draw_decimal (screen, game->lifebox->left, game->lifebox->top, n);
}

static void
draw_field (SDL_Surface* const screen, motion_t* const game)
{
    box_t* field = game->field;
    draw_fillbox (screen,
        field->left - 4, field->top - 4, field->right + 4, field->bottom,
        255, 255, 255);
    draw_fillbox (screen,
        field->left, field->top, field->right, field->bottom,
        128, 128, 128);
}

static void
draw_ball (SDL_Surface* const screen, motion_t* const game)
{
    if (game->ball_life <= 0 || game->brick_count <= 0)
        return;

    box_t* ball = game->ball;
    int const w = (ball->right - ball->left) / 2;
    int const h = (ball->bottom - ball->top) / 2;
    draw_fillcircle (screen,
        ball->left + w, ball->top + h, w,
        250, 250, 250);
}

static void
draw_paddle (SDL_Surface* const screen, motion_t* const game)
{
    box_t* paddle = game->paddle;
    draw_fillbox (screen,
        paddle->left, paddle->top, paddle->right, paddle->bottom,
        250, 250, 250);
}

static void
draw_bricks (SDL_Surface* const screen, motion_t* const game)
{
    for (int j = 0; j < game->grid_vsize; j++) {
        for (int i = 0; i < game->grid_hsize; i++) {
            box_t* const brick = ref_brick (game, i, j);
            if (brick->alive)
                draw_fillbox (screen,
                    brick->left, brick->top, brick->right, brick->bottom,
                    250, 200, 200);
        }
    }
}

static void
draw_decimal (SDL_Surface* const screen, int const x, int const y, int n)
{
    int r = n % 10;
    int q = n / 10;
    for (int i = 7; i >= 0; i--) {
        draw_digit (screen, x + 16 * i, y, r + '0');
        if (q == 0)
            break;
        r = q % 10;
        q = q / 10;
    }
}

static void
draw_digit (SDL_Surface* const screen, int const x, int const y, char ch)
{
    int k = ch - '0';
    for (int j = 0; j < FONT_HEIGHT; j++) {
        int bits = font7x5[k][j];
        for (int i = 0; i < FONT_WIDTH; i++) {
            if (bits & (1 << (4 - i))) {
                int const h = x + 3 * i;
                int const v = y + 3 * j;
                draw_fillbox (screen, h, v, h + 1, v + 1, 43, 43, 43);
            }
        }
    }
}

static void
draw_fillbox (SDL_Surface* const screen,
    int left, int top, int right, int bottom, int r, int g, int b)
{
    SDL_Rect rect;

    rect.x = left;
    rect.y = top;
    rect.w = right - left + 1;
    rect.h = bottom - top + 1;
    SDL_FillRect (screen, &rect, SDL_MapRGB(screen->format, r, g, b));
}

/* see
 * http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
 */
static void
draw_fillcircle (SDL_Surface* const screen,
    int x0, int y0, int radius, int r, int g, int b)
{
    SDL_Rect rect;
    Uint32 color = SDL_MapRGB(screen->format, r, g, b);

    int x = radius;
    int y = 0;
    int e = 1 - x;
    while (x >= y) {
        rect.x = x0 - x; rect.y = y0 + y; rect.w = 2 * x; rect.h = 1;
        SDL_FillRect (screen, &rect, color);
        rect.x = x0 - x; rect.y = y0 - y; rect.w = 2 * x; rect.h = 1;
        SDL_FillRect (screen, &rect, color);
        rect.x = x0 - y; rect.y = y0 + x; rect.w = 2 * y; rect.h = 1;
        SDL_FillRect (screen, &rect, color);
        rect.x = x0 - y; rect.y = y0 - x; rect.w = 2 * y; rect.h = 1;
        SDL_FillRect (screen, &rect, color);
        y++;
        if (e < 0) {
            e += 2 * y + 1;
        }
        else {
            x--;
            e += 2 * (y - x + 1);
        }
    }
}

