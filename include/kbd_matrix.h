#ifndef KEYBOARD_MATRIX_H
#define KEYBOARD_MATRIX_H

#include <stdint.h>

extern void kbd_matrix_init();
extern void kbd_matrix_update();

extern unsigned int kbd_flags;
extern uint8_t kbd_matrix[10];

#endif