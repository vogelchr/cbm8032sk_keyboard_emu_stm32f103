#include "pet_kbd.h"

const unsigned char pet_kbd_table_german[128] = {
    [8] = PET_KEY_E(8),                  /* Backspace */
    [13] = PET_KEY_E(3),                 /* Return */
    [32] = PET_KEY_C(8),                 /* SPACE  */
    [33] = PET_KEY_SHIFT | PET_KEY_A(1), /* ! */
    [34] = PET_KEY_SHIFT | PET_KEY_A(0), /* " */
    [35] = PET_KEY_SHIFT | PET_KEY_B(9), /* # (paragraph sign!) */
    [36] = PET_KEY_SHIFT | PET_KEY_B(1), /* $ */
    [37] = PET_KEY_SHIFT | PET_KEY_B(0), /* % */
    [38] = PET_KEY_SHIFT | PET_KEY_C(9), /* & */
                                         /* [39] ' */
    [40] = PET_KEY_SHIFT | PET_KEY_C(0), /* ( */
    [41] = PET_KEY_SHIFT | PET_KEY_D(9), /* ) */
                                         /* [42] * */
                                         /* [43] + */
    [44] = PET_KEY_D(7),                 /* , */
    [45] = PET_KEY_G(8),                 /* - */
    [46] = PET_KEY_D(6),                 /* . */
    [47] = PET_KEY_SHIFT | PET_KEY_C(1), /* / */
    [48] = PET_KEY_D(1),                 /* 0 */
    [49] = PET_KEY_A(1),                 /* 1 */
    [50] = PET_KEY_A(0),                 /* 2 */
    [51] = PET_KEY_B(9),                 /* 3 */
    [52] = PET_KEY_B(1),                 /* 4 */
    [53] = PET_KEY_B(0),                 /* 5 */
    [54] = PET_KEY_C(9),                 /* 6 */
    [55] = PET_KEY_C(1),                 /* 7 */
    [56] = PET_KEY_C(0),                 /* 8 */
    [57] = PET_KEY_D(9),                 /* 9 */
    [58] = PET_KEY_SHIFT | PET_KEY_D(6), /* : */
    [59] = PET_KEY_SHIFT | PET_KEY_D(7), /* ; */
    [60] = PET_KEY_A(9),                 /* < */
    [61] = PET_KEY_SHIFT | PET_KEY_D(1), /* = */
    [62] = PET_KEY_SHIFT | PET_KEY_A(9), /* > */
    [63] = PET_KEY_SHIFT | PET_KEY_F(9), /* ? */
                                         /* [64] @ */
    [65] = PET_KEY_SHIFT | PET_KEY_A(3), /* A */
    [66] = PET_KEY_SHIFT | PET_KEY_C(6), /* B */
    [67] = PET_KEY_SHIFT | PET_KEY_B(6), /* C */
    [68] = PET_KEY_SHIFT | PET_KEY_B(3), /* D */
    [69] = PET_KEY_SHIFT | PET_KEY_B(5), /* E */
    [70] = PET_KEY_SHIFT | PET_KEY_C(2), /* F */
    [71] = PET_KEY_SHIFT | PET_KEY_C(3), /* G */
    [72] = PET_KEY_SHIFT | PET_KEY_D(2), /* H */
    [73] = PET_KEY_SHIFT | PET_KEY_F(3), /* I */
    [74] = PET_KEY_SHIFT | PET_KEY_D(3), /* J */
    [75] = PET_KEY_SHIFT | PET_KEY_F(2), /* K */
    [76] = PET_KEY_SHIFT | PET_KEY_F(3), /* L */
    [77] = PET_KEY_SHIFT | PET_KEY_D(8), /* M */
    [78] = PET_KEY_SHIFT | PET_KEY_C(7), /* N */
    [79] = PET_KEY_SHIFT | PET_KEY_F(5), /* O */
    [80] = PET_KEY_SHIFT | PET_KEY_G(4), /* P */
    [81] = PET_KEY_SHIFT | PET_KEY_A(5), /* Q */
    [82] = PET_KEY_SHIFT | PET_KEY_C(4), /* R */
    [83] = PET_KEY_SHIFT | PET_KEY_B(2), /* S */
    [84] = PET_KEY_SHIFT | PET_KEY_C(5), /* T */
    [85] = PET_KEY_SHIFT | PET_KEY_D(5), /* U */
    [86] = PET_KEY_SHIFT | PET_KEY_B(7), /* V */
    [87] = PET_KEY_SHIFT | PET_KEY_B(4), /* W */
    [88] = PET_KEY_SHIFT | PET_KEY_B(9), /* X */
    [89] = PET_KEY_SHIFT | PET_KEY_A(7), /* Y */
    [90] = PET_KEY_SHIFT | PET_KEY_D(4), /* Z */
    [91] = PET_KEY_E(5),                 /* [ */
                                         /* [92] \ */
    [93] = PET_KEY_H(4),                 /* ] */
    [94] = PET_KEY_SHIFT | PET_KEY_E(5), /* ^ */
    [95] = PET_KEY_SHIFT | PET_KEY_G(8), /* _ */
                                         /* [96] ` */
    [97] = PET_KEY_A(3),                 /* a */
    [98] = PET_KEY_C(6),                 /* b */
    [99] = PET_KEY_B(6),                 /* c */
    [100] = PET_KEY_B(3),                /* d */
    [101] = PET_KEY_B(5),                /* e */
    [102] = PET_KEY_C(2),                /* f */
    [103] = PET_KEY_C(3),                /* g */
    [104] = PET_KEY_D(2),                /* h */
    [105] = PET_KEY_F(4),                /* i */
    [106] = PET_KEY_D(3),                /* j */
    [107] = PET_KEY_F(2),                /* k */
    [108] = PET_KEY_F(3),                /* l */
    [109] = PET_KEY_D(8),                /* m */
    [110] = PET_KEY_C(7),                /* n */
    [111] = PET_KEY_F(5),                /* o */
    [112] = PET_KEY_G(4),                /* p */
    [113] = PET_KEY_A(5),                /* q */
    [114] = PET_KEY_C(4),                /* r */
    [115] = PET_KEY_B(2),                /* s */
    [116] = PET_KEY_C(5),                /* t */
    [117] = PET_KEY_D(5),                /* u */
    [118] = PET_KEY_B(7),                /* v */
    [119] = PET_KEY_B(4),                /* w */
    [120] = PET_KEY_B(8),                /* x */
    [121] = PET_KEY_A(7),                /* y */
    [122] = PET_KEY_D(4),                /* z */
                                         /* [123] { */
                                         /* [124] | */
                                         /* [125] } */
                                         /* [126] ~ */
};
