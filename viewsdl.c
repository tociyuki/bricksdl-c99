#include "box.h"
#include "motion.h"
#include "viewsdl.h"
#include "SDL/SDL.h"
#include "SDL/SDL_gfxPrimitives.h"

static char const chbmp[10][8] = {
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

void
draw_frame (SDL_Surface* const screen, motion_t* const game)
{
    SDL_FillRect (screen, NULL, 0xd9d9d9);
    draw_field (screen, game);
    draw_paddle (screen, game);
    draw_ball (screen, game);
    draw_bricks (screen, game);
    draw_decimal (screen, SCORE_H, SCORE_V, game->score);
    int const n = game->ball_life > 0 ? game->ball_life - 1 : 0;
    draw_decimal (screen, LIVES_H, LIVES_V, n);
}

static void
draw_field (SDL_Surface* screen, motion_t* const game)
{
    box_t* field = game->field;
    boxRGBA (screen,
        field->left - 4, field->top - 4, field->right + 4, field->bottom,
        255, 255, 255, 255);
    boxRGBA (screen,
        field->left, field->top, field->right, field->bottom,
        128, 128, 128, 255);
}

static void
draw_ball (SDL_Surface* screen, motion_t* const game)
{
    if (game->ball_life <= 0 || game->brick_count <= 0)
        return;

    box_t* ball = game->ball;
    int const w = (ball->right - ball->left) / 2;
    int const h = (ball->bottom - ball->top) / 2;
    filledEllipseRGBA (screen,
        ball->left + w, ball->top + h, w, h,
        250, 250, 250, 255);
}

static void
draw_paddle (SDL_Surface* screen, motion_t* const game)
{
    box_t* paddle = game->paddle;
    boxRGBA (screen,
        paddle->left, paddle->top, paddle->right, paddle->bottom,
        250, 250, 250, 255);
}

static void
draw_bricks (SDL_Surface* screen, motion_t* const game)
{
    for (int j = 0; j < game->grid_vsize; j++) {
        for (int i = 0; i < game->grid_hsize; i++) {
            box_t* const brick = ref_brick (game, i, j);
            if (brick->alive)
                boxRGBA (screen,
                    brick->left, brick->top, brick->right, brick->bottom,
                    250, 200, 200, 255);
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
    for (int j = 0; j < 7; j++) {
        int bits = chbmp[k][j];
        for (int i = 0; i < 5; i++) {
            if (bits & (1 << (4 - i))) {
                int const h = x + 3 * i;
                int const v = y + 3 * j;
                boxRGBA (screen, h, v, h + 1, v + 1, 43, 43, 43, 255);
            }
        }
    }
}

