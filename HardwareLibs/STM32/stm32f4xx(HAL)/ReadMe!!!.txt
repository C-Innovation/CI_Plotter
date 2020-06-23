**************************************************
*											*
*			STM32F4xx Library (HAL)			*
*											*
**************************************************

Hardware requirements:
	- USART1 With interrupt(TX&RX)	(*)
	- TIMER2 (For microseconds counter)
	- USB-CDC (*)

The next HAL Libraries must be included:
	- stm32f4xx_hal_tim.h
	- stm32f4xx_hal.h
	- stm32f4xx_hal_uart.h (*)
	- usbd_cdc_if.h (*)

(*) Note: Select one!

Tested on:
	- STM32F411CEU6 (https://aliexpress.ru/item/4000679874594.html?spm=a2g0o.productlist.0.0.18df4639UlzL37&algo_pvid=cc3f4916-81cf-490b-b7c2-9ee9c5a1862f&algo_expid=cc3f4916-81cf-490b-b7c2-9ee9c5a1862f-0&btsid=0b8b036d15929067764222510e4b16&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_)
		UART working OK
		USB-CDC working OK 
	- STM32F429ZIT6(STM32F429I-Disco https://www.st.com/en/evaluation-tools/32f429idiscovery.html)
		UART working OK
		USB-CDC working OK 