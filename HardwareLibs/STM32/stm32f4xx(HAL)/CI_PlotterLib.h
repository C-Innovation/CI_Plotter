/*
 * CI_PlotterLib.h
 *
 *  Created on: 28 апр. 2020 г.
 *      Author: ivanov.y
 */

#ifndef CI_PLOTTERLIB_H_
#define CI_PLOTTERLIB_H_

#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"


#define _USE_UART
//#define _USE_USB

#ifdef _USE_UART
#define HAL_UART_MODULE_ENABLED
#include "stm32f4xx_hal_uart.h"
#ifdef _USE_USB
#error "UART and USB don't work together! Select one!"
#endif
#endif

#ifdef _USE_USB
#include "usbd_cdc_if.h"
#ifdef _USE_UART
#error "UART and USB don't work together! Select one!"
#endif
#endif




typedef enum{
	PLOTTER_MODE_SIMPLE = 0,
	PLOTTER_MODE_REAL_TIME = 1,
}CI_PlotterModeTypeDef;

typedef enum
{
    PLOT_CHANNEL_0      = 0,
    PLOT_CHANNEL_1      = 1,
    PLOT_CHANNEL_2      = 2,
    PLOT_CHANNEL_3      = 3,
    PLOT_CHANNEL_4      = 4,
    PLOT_CHANNEL_5      = 5,
    PLOT_CHANNEL_6      = 6,
    PLOT_CHANNEL_7      = 7
}CI_PlotterCannelsTypeDef;

typedef union {
    float value;
    uint8_t bytes[4];
}mFloatToByteConverter;

#define MAX_TEXT_LENGHT			128

#define TERMINAL_COLOR_RED      "FF0000"
#define TERMINAL_COLOR_GREEN    "00FF00"
#define TERMINAL_COLOR_BLUE     "0000FF"
#define TERMINAL_COLOR_YELLOW   "FFFF00"
#define TERMINAL_COLOR_MAGENTA  "FF007F"
#define TERMINAL_COLOR_CYAN     "00FFFF"
#define TERMINAL_COLOR_WHITE    "FFFFFF"
#define TERMINAL_COLOR_PINK     "FF00FF"
#define TERMINAL_COLOR_GRAY     "808080"

#define COMMAND_RESET_ALL       "~]--reset-a"
#define COMMAND_RESET_PLOTTER   "~]--reset-p"
#define COMMAND_RESET_TERMINAL  "~]--reset-t"


#ifdef _USE_USB
//#define USB_CDC_BAUD_RATE		8000000
#endif


uint8_t CI_PlotterInit(volatile uint32_t speed);
uint8_t CI_PlotterPlot(volatile CI_PlotterCannelsTypeDef channel,volatile double value);
uint8_t CI_PlotterPrintln(char * message, char * color);
uint8_t CI_PlotterPrint(char * message, char * color);
uint8_t CI_PlotterPrintCommand(char * command);



#endif /* CI_PLOTTERLIB_H_ */
