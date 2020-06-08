#ifndef __CI_ARDUINOPLOTTER_H
#define __CI_ARDUINOPLOTTER_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Arduino.h"
#include <HardwareSerial.h>

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
}plotCannelsTypeDef;

typedef union {
    float value;
    uint8_t bytes[4];
}mFloatToByteConverter;

typedef union {
    uint32_t value;
    uint8_t bytes[4];
}mUint32ToByteConverter;

class CI_ArduinoPlotter
{
    public:
    CI_ArduinoPlotter(unsigned long baud = 115200);
    void PlotStr(plotCannelsTypeDef channel, double point);
    void Plot(plotCannelsTypeDef channel, float point);
    void Println( String message, String color = TERMINAL_COLOR_GRAY);
    void Print( String message, String color = TERMINAL_COLOR_GRAY);
    void PrintCommand(String command);

    private:
    bool realTimeEn;
    uint32_t millisOffset;
    //Serial() serial;
};

#endif