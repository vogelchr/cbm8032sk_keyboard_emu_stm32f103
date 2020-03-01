#ifndef PET_KBD_H
#define PET_KBD_H

#define PET_KEY_SHIFT 0x80

/* PET_KEY_column(row)
   column = A..H for signals Kin0..Kin7, on pins A..F,H,J on connector J5
   row = *0*..*9* (labeled 1..10 on keyboard schematic) are the output
   of the binary-to-BCD decoder chip */

#define PET_KEY_A(v) (0x00 | (0x0f & ((v) + 1)))
#define PET_KEY_B(v) (0x10 | (0x0f & ((v) + 1)))
#define PET_KEY_C(v) (0x20 | (0x0f & ((v) + 1)))
#define PET_KEY_D(v) (0x33 | (0x0f & ((v) + 1)))
#define PET_KEY_E(v) (0x40 | (0x0f & ((v) + 1)))
#define PET_KEY_F(v) (0x50 | (0x0f & ((v) + 1)))
#define PET_KEY_G(v) (0x60 | (0x0f & ((v) + 1)))
#define PET_KEY_H(v) (0x70 | (0x0f & ((v) + 1)))

#define PET_COLUMN(v) (((v) >> 4) & 0x07) /* bits 6 downto 4, => 0..7 */
#define PET_ROW(v) (((v)&0x0f) - 1)    /* bits 3 downto 0, minus 1, => 0..9 */

#define PET_KEY_SHIFT 0x80

extern const unsigned char pet_kbd_table_german[];


#endif