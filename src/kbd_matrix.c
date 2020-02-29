#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include <string.h>

/*
 * *** CBM8032SK Keyboard Connector ***
 *
 *   Physical Connector: D-SUB25, compouter side female, keyboard side male
 *
 *   Pins 1..10 correspond to J5 (on the PCB) pins 1..10, those are
 *      the open-collector, low-active outputs of the BCD to decimal
 *      decoder UC11, 74LS145. Inputs of the '145 are PA0..PA3 of UB12.
 *
 *   Pins 14..21 correspond to J5 (on the PCB) pins A..H, J
 *      those have a 10kOhm pullup to 5V and are the keyboard matrix
 *      sensing outputs, connected to PB0 of UB12 a 6520 PIA.
 *
 *   Pin 22 is Ground.
 *
 *   View onto computer front, pin 1 is on upper left.
 * 
 *                                                            8032 Connector
 *                 10  9   8   7   6   5   4   3   2   1    <- J5 Keybd. Scan
 *               10|   |   |   |   |   |   |   |   |   |      
 *     o   o   o   o   o   o   o   o   o   o   o   o   o(1) <- D-Sub
 *       o   o   o   o   o   o   o   o   o   o   o   o
 *                 22|   |   |   |   |   |   |   | 14|
 *                  12   J   H   F   E   D   C   B   A      <- J5
 *                      Kin7                        Kin0
 *
 * *** STM32 ***
 * 
 *   The 8 inputs should be 5V tolerant (they have 5V pullups, after all),
 *   the 10 inputs can be 3.3V as they are only pulled low. We need some
 *   (weak) pullups on those inputs.
 *
 *   PA0..PA7, PB0..PB1 on the stm32 will be connceted to the
 *       scan rows, the active low outputs of the BCD decoder.
 *
 *                column                                               column
 *                Kin0,1                                       Row     Kin4..7
 *               |<--->|                                     |<--->| |<----------->|
 *  +-------------------------------------------------------------------------------+
 *  |3v3 GND 5v0 PB9 PB8 PB7 PB6 PB5 PB4 PB3 A15 A12 A11 A10 PA9 PA8 B14 B13 B13 B12|
 *  |                                                                               |
 *  | SWD          "BluePill" module                                            USB |
 *  |                                                                               |
 *  |BAT C13 C14 C15 PA0 PA1 PA2 PA3 PA4 PA5 PA6 PA7 PB0 PB1 B10 B11 RST 3v3 GND GND|
 *  +-------------------------------------------------------------------------------+
 *                   |<--------------------------->|         |<--->|
 *                      Keyboard Scan Rows                    Kin2,3
 *                      only one active low signal at         column
 *                      a time
 */

/* kbd_matrix[row] = 1<<col, a set bit corresponds to a pushed key */
uint8_t kbd_matrix[10];

unsigned int kbd_flags; /* mainly for debugging, stores current state of I/O */

void kbd_matrix_update(void)
{
	int i;
	uint16_t col_ret;
	uint16_t row_sel;

	/* row selection is via GPIOA, bits 0..9, active low! */
	row_sel = gpio_get(GPIOA, 0x03ff);
	/* column return will be the wired and of rows */
	col_ret = 0xff;

	for (i = 0; i < 10; i++)
	{
		/* check if select line for this row is logic low */
		if (!(row_sel & (1UL << i)))
		{
			kbd_flags |= (1UL << i);
			col_ret &= ~kbd_matrix[i];
		}
	}

	kbd_flags = (row_sel & 0x3ff) << 20 | col_ret;

	gpio_clear(GPIOB, 0xff00 & ~(col_ret << 8));
	gpio_set(GPIOB, 0xff00 & (col_ret << 8));
}

void kbd_matrix_init(void)
{
	memset(kbd_matrix, '\0', sizeof(kbd_matrix));

	gpio_set_mode(GPIOA,
		      GPIO_MODE_INPUT,
		      GPIO_CNF_INPUT_PULL_UPDOWN,
		      0x03ff);
	gpio_set(GPIOA, 0x03ff); /* enable pull-ups */

	gpio_set_mode(GPIOB,
		      GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_OPENDRAIN,
		      0xff00);
	gpio_set(GPIOB, 0xff00);
}
