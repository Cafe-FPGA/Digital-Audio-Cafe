#include "main.h"
#include <stdio.h>

extern UART_HandleTypeDef huart1; // Hardware configuration is in main.c
char uart_buf[20]; // DMA

extern "C" void app_main()
{

	/* USER CODE BEGIN 2 */
	int volume = 7; // 15 max
	int last_volume = -1; // clone of volume to save UART bandwidth
	int saved_volume = 7;
	bool is_muted = false;
	int last_enc_button_state = 1;
	int last_clock_state = 1;
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while(1)
	{
		/* USER CODE END WHILE */
		/* USER CODE BEGIN 3 */

		// Beginning of volume, mute, and relevant UART transmission logic
		int clock_state = HAL_GPIO_ReadPin(ENC_CLK_GPIO_Port, ENC_CLK_Pin);
		int data_state = HAL_GPIO_ReadPin(ENC_DT_GPIO_Port, ENC_DT_Pin);
		int enc_button_state = HAL_GPIO_ReadPin(ENC_SW_GPIO_Port, ENC_SW_Pin);

		if (last_clock_state == 1 && clock_state == 0)
		{
		  if (data_state == 0)
		  {
			  // Clockwise, stop at 15
			  if (volume < 15)
			  {
				  volume += 1;
			  }
		  } else
		  {
			  // Counter clockwise, stop at 0
			  if (volume > 0)
			  {
				  volume -= 1;
			  }
		  }
		} // End volume change logic

		if (last_enc_button_state == 1 && enc_button_state == 0)
		{
			if (is_muted == false)
			{
				is_muted = true;
				saved_volume = volume;
				volume = 0;
			} else
			{
				is_muted = false;
				volume = saved_volume;
			}
		} // End mute logic

		if (volume != last_volume)
		{
		  int len = sprintf(uart_buf, "VOL:%d\n", volume);
		  HAL_UART_Transmit_DMA(&huart1, (uint8_t*)uart_buf, len);
		  //HAL_UART_Transmit(&huart1, (uint8_t*)uart_buf, len, 100); // DEBUG FOR UART COMMUNICATION ----------
		  last_volume =  volume; // Update clone
		} // End UART volume transmit

		last_clock_state = clock_state; // Limits volume change to once per encoder click
		last_enc_button_state = enc_button_state; // Limits muting to once per encoder press
		// End of volume, mute, and relevant UART transmission logic

	}
	/* USER CODE END 3 */
}
