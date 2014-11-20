#ifndef BOX_H

#define BOX_H

#include <stdbool.h>

typedef struct box_s {
    int left;
    int top;
    int right;
    int bottom;
    int hspeed;
    int vspeed;
    bool alive;
} box_t;

box_t* set_box (int const left, int const top,
    int const right, int const bottom,
    int const hspeed, int const vspeed, box_t* box);

box_t* set_boxspeed (int const hspeed, int const vspeed, box_t* const box);

int box_hcenter (box_t* const box);

int box_vcenter (box_t* const box);

box_t* offset_box (box_t* const src, int const dh, int const dv, box_t* const dst);

box_t* union_box (box_t* const src0, box_t* const src1, box_t* const dst);

bool sect_box (box_t* const src0, box_t* const src1, box_t* dst);

void clip_box (box_t* const field, box_t* const box);

#endif

