/*
 * CI_PlotterLib.c
 *
 *  Created on: 28 апр. 2020 г.
 *      Author: ivanov.y
 */
#include "CI_PlotterLib.h"

#ifdef _USE_USB
#include "usbd_cdc_if.h"
#endif

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;
TIM_HandleTypeDef htim2;

uint8_t timFirstEnter = 1;

CI_PlotterModeTypeDef mainPlotterMode;

/************************************************************************************
 *  Configuration of UART1 and TIMER2 modules;                                      *
 *  Status: Available.                                                              *
 *  @param: speed   - COM Port baud rate		                                    *
 ************************************************************************************/
uint8_t CI_PlotterInit(volatile uint32_t speed)
{

	uint8_t res = 0;
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};
	__HAL_RCC_TIM2_CLK_ENABLE();
	//mainPlotterMode = mode;

    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();

	huart1.Instance = USART1;
	huart1.Init.BaudRate = speed;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		res++;
	}

	HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

	GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = ((HAL_RCC_GetHCLKFreq() / 1000000) - 1);
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 0xFFFFFFFF;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		res++;
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	{
		res++;
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		res++;
	}

	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	HAL_TIM_Base_Start(&htim2);

	HAL_Delay(1000);
	res += CI_PlotterPrintCommand(COMMAND_RESET_PLOTTER);
	return res;
}

/************************************************************************************
 *  Send point to serial port;                                                      *
 *  Status: Available.                                                              *
 *  @param: channel - Listener Channel (PLOT_CHANNEL_0 - PLOT_CHANNEL_7)            *
 *  @param: point   - Point for plotter (Y value)                                   *
 ************************************************************************************/
uint8_t CI_PlotterPlot(volatile CI_PlotterCannelsTypeDef channel,volatile double value)
{
	uint8_t res = 0;
	uint8_t out[14] = {0};
	uint32_t micros = (TIM2->CNT);
	uint8_t size = 0;
    mFloatToByteConverter converterFloat;
    converterFloat.value = value;

	out[0] = 0x23;
	out[1] = 0x50;
	out[2] = 0x4C;
	out[3] = (uint8_t)channel;
	out[4] = (micros >> 24) & 0xff;
	out[5] = (micros >> 16) & 0xff;
	out[6] = (micros >> 8) & 0xff;
	out[7] = (micros) & 0xff;
	out[8] = converterFloat.bytes[0];
	out[9] = converterFloat.bytes[1];
	out[10] = converterFloat.bytes[2];
	out[11] = converterFloat.bytes[3];
	out[12] = 0x5B;
	out[13] = 0x7E;
	size = 14;



#ifdef _USE_USB
#if DEVICE_FS
	res += CDC_Transmit_FS(&out[0], size);
#else if DEVICE_HS
	res += CDC_Transmit_HS(&out[0], size);
#endif
#else
	while(HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);
	res += HAL_UART_Transmit(&huart1, &out[0], size, 1000);
#endif
	return res;
}

/************************************************************************************
 *  Send message to serial port with "LineFeed";                                    *
 *  Status: Available.                                                              *
 *  @param: message - Listener Message for terminal                                 *
 *  @param: color   - Color for listener terminal(24bit in string format)           *
 ************************************************************************************/
uint8_t CI_PlotterPrintln(char * message, char * color)
{
	uint8_t res = 0;
	uint8_t str[MAX_TEXT_LENGHT + 15] = {0};
	uint8_t str2[128] = {0};
	uint8_t size = 0;
	sprintf(&str[0],"#TL|%s|%s\n[~",color, message);

	size = strlen(message);
	if(size >= MAX_TEXT_LENGHT)
	{
		sprintf(&str2[0],"#TL|%s|[CI_PlotterLib]\t\tThe length of message is too big! MAX_TEXT_LENGHT = %d[~",TERMINAL_COLOR_RED, MAX_TEXT_LENGHT);
		size = strlen(str2);
#ifdef _USE_USB
#if DEVICE_FS
	res += CDC_Transmit_FS(&str2[0], size);
#else if DEVICE_HS
	res += CDC_Transmit_HS(&str2[0], size);
#endif
#else
		while(HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);
		res += HAL_UART_Transmit(&huart1, (uint8_t*)str2, size, 1000);
#endif
		return 1;
	}
	size = strlen(str);
#ifdef _USE_USB
#if DEVICE_FS
	res += CDC_Transmit_FS(&str[0], size);
#else if DEVICE_HS
	res += CDC_Transmit_HS(&str[0], size);
#endif
#else
	while(HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);
	res += HAL_UART_Transmit(&huart1, (uint8_t*)str, size, 1000);
#endif
	return res;
}

/************************************************************************************
 *  Send message to serial port without "LineFeed";                                 *
 *  Status: Available.                                                              *
 *  @param: message - Listener Message for terminal                                 *
 *  @param: color   - Color for listener terminal(24bit in string format)           *
 ************************************************************************************/
uint8_t CI_PlotterPrint(char * message, char * color)
{
	uint8_t res = 0;
	uint8_t str[MAX_TEXT_LENGHT + 15] = {0};
	uint8_t str2[128] = {0};
	uint8_t size = 0;
	sprintf(&str[0],"#TS|%s|%s[~",color, message);
	if(size >= MAX_TEXT_LENGHT)
	{
		sprintf(&str2[0],"#TL|%s|[CI_PlotterLib]\t\tThe length of message is too big! MAX_TEXT_LENGHT = %d[~",TERMINAL_COLOR_RED, MAX_TEXT_LENGHT);
		size = strlen(str2);
#ifdef _USE_USB
#if DEVICE_FS
	res += CDC_Transmit_FS(&str2[0], size);
#else if DEVICE_HS
	res += CDC_Transmit_HS(&str2[0], size);
#endif
#else
		while(HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);
		res += HAL_UART_Transmit(&huart1, (uint8_t*)str2, size, 1000);
#endif
		return 1;
	}
	size = strlen(str);

#ifdef _USE_USB
#if DEVICE_FS
	res += CDC_Transmit_FS(&str[0], size);
#else if DEVICE_HS
	res += CDC_Transmit_HS(&str[0], size);
#endif
#else
	while(HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);
	res += HAL_UART_Transmit(&huart1, (uint8_t*)str, size, 1000);
#endif
	return res;
}

/************************************************************************************
 *  Send command to serial port for Listener;                                       *
 *  Status: Available.                                                              *
 *  @param: command - Listener command                                              *
 ************************************************************************************/
uint8_t CI_PlotterPrintCommand(char * command)
{
	uint8_t res = 0;
	uint8_t str[50] = {0};
	uint8_t size = 0;
	sprintf(&str[0],"#TL|%s|%s[~",TERMINAL_COLOR_CYAN, command);
	size = strlen(str);
#ifdef _USE_USB
#if DEVICE_FS
	res += CDC_Transmit_FS(&str[0], size);
#else if DEVICE_HS
	res += CDC_Transmit_HS(&str[0], size);
#endif
#else
	while(HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);
	res += HAL_UART_Transmit(&huart1, (uint8_t*)str, size, 1000);
#endif
	return res;
}



