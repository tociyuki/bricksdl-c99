#include "box.h"
#include <stddef.h>
#include <stdbool.h>

box_t*
set_box (int const left, int const top, int const right, int const bottom,
    int const hspeed, int const vspeed, box_t* box)
{
    box->left = left;
    box->top = top;
    box->right = right;
    box->bottom = bottom;
    box->hspeed = hspeed;
    box->vspeed = vspeed;
    return box;
}

box_t*
set_boxspeed (int const hspeed, int const vspeed, box_t* const box)
{
    box->hspeed = hspeed;
    box->vspeed = vspeed;
    return box;
}

int
box_hcenter (box_t* const box)
{
    return (box->left + box->right) / 2;
}

int
box_vcenter (box_t* const box)
{
    return (box->top + box->bottom) / 2;
}

box_t*
offset_box (box_t* const src, int const dh, int const dv, box_t* const dst)
{
    dst->left = src->left + dh;
    dst->top = src->top + dv;
    dst->right = src->right + dh;
    dst->bottom = src->bottom + dv;
    dst->hspeed = src->hspeed;
    dst->vspeed = src->vspeed;
    return dst;
}

box_t*
union_box (box_t* const src0, box_t* const src1, box_t* const dst)
{
    dst->left   = src0->left   < src1->left   ? src0->left   : src1->left;
    dst->top    = src0->top    < src1->top    ? src0->top    : src1->top;
    dst->right  = src0->right  > src1->right  ? src0->right  : src1->right;
    dst->bottom = src0->bottom > src1->bottom ? src0->bottom : src1->bottom;
    return dst;
}

bool
sect_box (box_t* const src0, box_t* const src1, box_t* dst)
{
    box_t dummy;

    if (dst == NULL)
        dst = &dummy;
    dst->left   = src0->left   > src1->left   ? src0->left   : src1->left;
    dst->top    = src0->top    > src1->top    ? src0->top    : src1->top;
    dst->right  = src0->right  < src1->right  ? src0->right  : src1->right;
    dst->bottom = src0->bottom < src1->bottom ? src0->bottom : src1->bottom;
    return dst->left <= dst->right && dst->top <= dst->bottom;
}

void
clip_box (box_t* const field, box_t* const box)
{
    int const gh = box->hspeed;
    int const gv = box->vspeed;
    for (;;) {
        if (field->left > box->left) {
            int const dh = field->left - box->left;
            offset_box (box, dh, dh * gv / gh, box);
        }
        else if (field->right < box->right) {
            int const dh = field->right - box->right;
            offset_box (box, dh, dh * gv / gh, box);
        }
        else if (field->top > box->top) {
            int const dv = field->top - box->top;
            offset_box (box, dv * gh / gv, dv, box);
        }
        else if (field->bottom < box->bottom) {
            int const dv = field->bottom - box->bottom;
            offset_box (box, dv * gh / gv, dv, box);
        }
        else {
            break;
        }
    }
}

