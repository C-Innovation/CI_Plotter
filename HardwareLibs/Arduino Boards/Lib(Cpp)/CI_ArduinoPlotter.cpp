
#include "CI_ArduinoPlotter.h"

// Already contains Serial.begin()!!!!!!
CI_ArduinoPlotter::CI_ArduinoPlotter(unsigned long baud)
{
    Serial.begin(baud);
    millisOffset = millis();
    PrintCommand(COMMAND_RESET_PLOTTER);
}

/************************************************************************************
 *  Send point to serial port;                                                      *
 *  Status: Available.                                                              *
 *  @param: channel - Listener Channel (PLOT_CHANNEL_0 - PLOT_CHANNEL_7)            *
 *  @param: point   - Point for plotter (Y value)                                   *
 ************************************************************************************/
void CI_ArduinoPlotter::Plot(plotCannelsTypeDef channel, float point)
{
    if((uint8_t)channel > 7)
        return;
    // uint8_t out[18] = {0};
    uint8_t out[14] = {0};
    uint32_t micr = (uint32_t)(micros() & 0xffffffff);
    mFloatToByteConverter converterFloat;
    mUint32ToByteConverter converterUint32;
    converterUint32.value = micr;
    converterFloat.value = point;

    out[0] = 0x23;
    out[1] = 0x50;
    out[2] = 0x4C;
    out[3] = (uint8_t)channel;

    out[4] = (micr >> 24) & 0xff;
    out[5] = (micr >> 16) & 0xff;
    out[6] = (micr >> 8) & 0xff;
    out[7] = (micr) & 0xff;
    out[8] = converterFloat.bytes[0];
    out[9] = converterFloat.bytes[1];
    out[10] = converterFloat.bytes[2];
    out[11] = converterFloat.bytes[3];
    out[12] = 0x5B;
    out[13] = 0x7E;
    
    Serial.write(out,14);
}

/************************************************************************************
 *  Description: Send point to serial port in string mode(VERY-VERY SLOW!!!!!!);    *
 *  Status: Unavailable.                                                            *
 *  @param: channel - Listener Channel (PLOT_CHANNEL_0 - PLOT_CHANNEL_7)            *
 *  @param: point   - Point for plotter (Y value)                                   *
 ************************************************************************************/
void CI_ArduinoPlotter::PlotStr(plotCannelsTypeDef channel, double point)
{
    String str = "";
    if(realTimeEn)
    {
        str += "#PLRT|CH";
    }
    else
    {
        str += "#PLOT|CH";
    }

    str += String(((int)channel),DEC);
    str += "|";
    if(realTimeEn)
    {
        str += String((uint32_t)(micros() & 0xffffffff),DEC);
        str += "|";
    }
    str += String(point,3);
    str += "[~";
    Serial.print(str);
}

/************************************************************************************
 *  Send message to serial port with "LineFeed";                                    *
 *  Status: Available.                                                              *
 *  @param: message - Listener Message for terminal                                 *
 *  @param: color   - Color for listener terminal(24bit in string format)           *
 ************************************************************************************/
void CI_ArduinoPlotter::Println( String message, String color)
{
    char messageBytes[message.length()] ;
    
    message.getBytes((unsigned char*)messageBytes,message.length());
    if(strstr(messageBytes,"[~") || strstr(messageBytes,"~]"))
         return;
         
    String str = "#TL|";
    str += color;
    str += "|";
    str += message;
    str += "\n[~";
    Serial.print(str);
}

/************************************************************************************
 *  Send message to serial port without "LineFeed";                                 *
 *  Status: Available.                                                              *
 *  @param: message - Listener Message for terminal                                 *
 *  @param: color   - Color for listener terminal(24bit in string format)           *
 ************************************************************************************/
void CI_ArduinoPlotter::Print( String message, String color)
{
    char messageBytes[message.length()] ;
    
    message.getBytes((unsigned char*)messageBytes,message.length());
    if(strstr(messageBytes,"[~") || strstr(messageBytes,"~]"))
         return;

    String str = "#TS|";
    str += color;
    str += "|";
    str += message;
    str += "[~";
    Serial.print(str);
}

/************************************************************************************
 *  Send command to serial port for Listener;                                       *
 *  Status: Available.                                                              *
 *  @param: command - Listener command                                              *
 ************************************************************************************/
void CI_ArduinoPlotter::PrintCommand(String command)
{
    String str = "#TL|";
    str += TERMINAL_COLOR_CYAN;
    str += "|";
    str += command;
    str += "[~";
    Serial.print(str);
}
