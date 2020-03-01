/*
 * This file is based on (initially 100% copied) from
 *    ibopencm3-examples/blob/master/examples/stm32/f1/waveshare-open103r/usbserial/usbserial.c
 * 
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>
 * Copyright (C) 2013 Joshua Harlan Lifton <joshua.harlan.lifton@gmail.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/cdc.h>

#include "ringbuf.h"
#include "kbd_matrix.h"
#include "pet_kbd.h"

static const struct usb_device_descriptor dev = {
    .bLength = USB_DT_DEVICE_SIZE,
    .bDescriptorType = USB_DT_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = USB_CLASS_CDC,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = 64,
    .idVendor = 0x0483,
    .idProduct = 0x5740,
    .bcdDevice = 0x0200,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1,
};

/*
 * This notification endpoint isn't implemented. According to CDC spec its
 * optional, but its absence causes a NULL pointer dereference in Linux
 * cdc_acm driver.
 */
static const struct usb_endpoint_descriptor comm_endp[] = {
    {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x83,
	.bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
	.wMaxPacketSize = 16,
	.bInterval = 255,
    }};

static const struct usb_endpoint_descriptor data_endp[] = {
    {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x01,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = 64,
	.bInterval = 1,
    },
    {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x82,
	.bmAttributes = USB_ENDPOINT_ATTR_BULK,
	.wMaxPacketSize = 64,
	.bInterval = 1,
    }};

static const struct
{
	struct usb_cdc_header_descriptor header;
	struct usb_cdc_call_management_descriptor call_mgmt;
	struct usb_cdc_acm_descriptor acm;
	struct usb_cdc_union_descriptor cdc_union;
} __attribute__((packed)) cdcacm_functional_descriptors = {
    .header = {
	.bFunctionLength = sizeof(struct usb_cdc_header_descriptor),
	.bDescriptorType = CS_INTERFACE,
	.bDescriptorSubtype = USB_CDC_TYPE_HEADER,
	.bcdCDC = 0x0110,
    },
    .call_mgmt = {
	.bFunctionLength = sizeof(struct usb_cdc_call_management_descriptor),
	.bDescriptorType = CS_INTERFACE,
	.bDescriptorSubtype = USB_CDC_TYPE_CALL_MANAGEMENT,
	.bmCapabilities = 0,
	.bDataInterface = 1,
    },
    .acm = {
	.bFunctionLength = sizeof(struct usb_cdc_acm_descriptor),
	.bDescriptorType = CS_INTERFACE,
	.bDescriptorSubtype = USB_CDC_TYPE_ACM,
	.bmCapabilities = 0,
    },
    .cdc_union = {
	.bFunctionLength = sizeof(struct usb_cdc_union_descriptor),
	.bDescriptorType = CS_INTERFACE,
	.bDescriptorSubtype = USB_CDC_TYPE_UNION,
	.bControlInterface = 0,
	.bSubordinateInterface0 = 1,
    }};

static const struct usb_interface_descriptor comm_iface[] = {
    {.bLength = USB_DT_INTERFACE_SIZE,
     .bDescriptorType = USB_DT_INTERFACE,
     .bInterfaceNumber = 0,
     .bAlternateSetting = 0,
     .bNumEndpoints = 1,
     .bInterfaceClass = USB_CLASS_CDC,
     .bInterfaceSubClass = USB_CDC_SUBCLASS_ACM,
     .bInterfaceProtocol = USB_CDC_PROTOCOL_AT,
     .iInterface = 0,
     .endpoint = comm_endp,
     .extra = &cdcacm_functional_descriptors,
     .extralen = sizeof(cdcacm_functional_descriptors)}};

static const struct usb_interface_descriptor data_iface[] = {
    {
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 1,
	.bAlternateSetting = 0,
	.bNumEndpoints = 2,
	.bInterfaceClass = USB_CLASS_DATA,
	.bInterfaceSubClass = 0,
	.bInterfaceProtocol = 0,
	.iInterface = 0,
	.endpoint = data_endp,
    }};

static const struct usb_interface ifaces[] = {
    {
	.num_altsetting = 1,
	.altsetting = comm_iface,
    },
    {
	.num_altsetting = 1,
	.altsetting = data_iface,
    }};

static const struct usb_config_descriptor config = {
    .bLength = USB_DT_CONFIGURATION_SIZE,
    .bDescriptorType = USB_DT_CONFIGURATION,
    .wTotalLength = 0,
    .bNumInterfaces = 2,
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes = 0x80,
    .bMaxPower = 0x32,
    .interface = ifaces,
};

static const char *usb_strings[] = {
    "github.com/vogelchr",
    "cbm8032sk keyboard emulator",
    "0000001",
};

/* Buffer to be used for control requests. */
uint8_t usbd_control_buffer[128];

static int
cdcacm_control_request(usbd_device *usbd_dev,
		       struct usb_setup_data *req,
		       uint8_t **buf,
		       uint16_t *len,
		       int (**complete)(usbd_device *usbd_dev,
					struct usb_setup_data *req))
{
	(void)complete;
	(void)buf;
	(void)usbd_dev;

	switch (req->bRequest)
	{
	case USB_CDC_REQ_SET_CONTROL_LINE_STATE:
	{
		/*
     * This Linux cdc_acm driver requires this to be implemented
     * even though it's optional in the CDC spec, and we don't
     * advertise it in the ACM functional descriptor.
     */
		char local_buf[10];
		struct usb_cdc_notification *notif = (void *)local_buf;

		/* We echo signals back to host as notification. */
		notif->bmRequestType = 0xA1;
		notif->bNotification = USB_CDC_NOTIFY_SERIAL_STATE;
		notif->wValue = 0;
		notif->wIndex = 0;
		notif->wLength = 2;
		local_buf[8] = req->wValue & 3;
		local_buf[9] = 0;
		// usbd_ep_write_packet(0x83, buf, 10);
		return USBD_REQ_HANDLED;
	}
	case USB_CDC_REQ_SET_LINE_CODING:
		if (*len < sizeof(struct usb_cdc_line_coding))
		{
			return USBD_REQ_NOTSUPP;
		}
		return USBD_REQ_HANDLED;
	}
	return 0;
}

static uint16_t kbd_queue[16];
static uint16_t *volatile kbd_queue_readp = kbd_queue;
static uint16_t *volatile kbd_queue_writep = kbd_queue;

void tim2_isr()
{
	char c;
	int row, col;

	timer_clear_flag(TIM2, TIM_SR_UIF);

	if (RINGBUF_GET(kbd_queue, kbd_queue_readp, kbd_queue_writep, &c))
		return;

	row = PET_ROW(c);
	col = PET_COLUMN(c);

	if (c & PET_KEY_SHIFT) /* abusing shift as press/release */
		kbd_matrix[row] |= (1UL << col);
	else
		kbd_matrix[row] &= ~(1UL << col);

	gpio_toggle(GPIOC, 1 << 13);
}

int serial_input_eat_char(unsigned char c)
{
	unsigned char kbd_code;
	uint16_t q;

	if (c < 128 && ((kbd_code = pet_kbd_table_german[c]) != 0))
	{

		if (kbd_code & PET_KEY_SHIFT) {
			q = PET_KEY_A(6) | PET_KEY_SHIFT; /* pree A6 = left shift */
			RINGBUF_PUT(kbd_queue, kbd_queue_readp, kbd_queue_writep, &q);
		}

		/* in the queue we are abusing the MSB for press/release! */
		q = kbd_code | PET_KEY_SHIFT; /* press */
		RINGBUF_PUT(kbd_queue, kbd_queue_readp, kbd_queue_writep, &q);
		q &= ~PET_KEY_SHIFT; /* release */
		RINGBUF_PUT(kbd_queue, kbd_queue_readp, kbd_queue_writep, &q);

		if (kbd_code & PET_KEY_SHIFT) {
			q = PET_KEY_A(6); /* release A6 = left shift */
			RINGBUF_PUT(kbd_queue, kbd_queue_readp, kbd_queue_writep, &q);
		}
	}
}

static char usb_rxbuf[64];
static char usb_txbuf[64];

static void cdcacm_data_rx_cb(usbd_device *usbd_dev, uint8_t ep)
{
	(void)ep;
	int len, i, j;

	len = usbd_ep_read_packet(usbd_dev, 0x01, usb_rxbuf, sizeof(usb_rxbuf));

	for (i = 0; i < len; i++)
	{
		char c = usb_rxbuf[i];
		if (c == '\1') /* ctrl-a */
		{
			j = snprintf(usb_txbuf, sizeof(usb_txbuf), "%p %p %p\r\n",
				     kbd_queue, kbd_queue_readp, kbd_queue_writep);
			usbd_ep_write_packet(usbd_dev, 0x82, usb_txbuf, j);
		}
		else
			serial_input_eat_char(c);
	}
}

static void cdcacm_set_config(usbd_device *usbd_dev, uint16_t wValue)
{
	(void)wValue;

	usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64, cdcacm_data_rx_cb);
	usbd_ep_setup(usbd_dev, 0x82, USB_ENDPOINT_ATTR_BULK, 64, NULL);
	usbd_ep_setup(usbd_dev, 0x83, USB_ENDPOINT_ATTR_INTERRUPT, 16, NULL);

	usbd_register_control_callback(usbd_dev,
				       USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
				       USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
				       cdcacm_control_request);
}

int main(void)
{
	usbd_device *usbd_dev;

	/* === system clock initialization ===
	   external 8MHz XTAL, SYSCLK=9(pll)*8MHz=72MHz, AHB 72MHz(max),
	   ADC 9MHz(14MHz max), APB1=36MHz(max), APB2=72MHz(max),
	   flash has 2 waitstates  */
	rcc_clock_setup_in_hse_8mhz_out_72mhz();

	/* === disable JTAG, enable SWD === */
	rcc_periph_clock_enable(RCC_AFIO);
	AFIO_MAPR |= AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_ON;

	/* === USB === */
	usbd_dev = usbd_init(&st_usbfs_v1_usb_driver,
			     &dev,
			     &config,
			     usb_strings,
			     3,
			     usbd_control_buffer,
			     sizeof(usbd_control_buffer));
	usbd_register_set_config_callback(usbd_dev, cdcacm_set_config);

	/* === Timer2 init === */
	rcc_periph_clock_enable(RCC_TIM2);
	rcc_periph_reset_pulse(RST_TIM2);
	nvic_enable_irq(NVIC_TIM2_IRQ);

	timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT,
		       TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	/* APB1 clock is 36MHz, but there is the TIMXCLK which
	   is APB2 clock x2 if the APB prescaler is used,
	   see STM32F1xx ref manual, Low-, medium-, high- and
	   XL-density reset and clock control (RCC),
	   page 93/1134 */

	timer_set_prescaler(TIM2, 36000); /* 36MHz * 2 / 36'000 = 2kHz  */
	timer_set_period(TIM2, 200);	  /* 2kHz / 200 = 10 Hz overflow */
	timer_enable_counter(TIM2);
	timer_enable_irq(TIM2, TIM_DIER_UIE);

	/* === debug LED === */
	rcc_periph_clock_enable(RCC_GPIOC);
	gpio_set_mode(GPIOC,
		      GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL,
		      GPIO13);
	gpio_clear(GPIOC, 1UL << 13);

	/* === keyboard matrix === */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	kbd_matrix_init();

	/* === main loop === */
	while (1)
	{
		kbd_matrix_update();
		usbd_poll(usbd_dev);
	}
}
