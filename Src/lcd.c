/*
 * lcd.c
 *
 *  Created on: 30 gru 2022
 *      Author: piotr
 */

//#include "lcd.h"
//#include "spi.h"
//
//uint16_t DMA_MIN_SIZE = 16;
//
//#define HOR_LEN 	5	//	Alse mind the resolution of your screen!
//uint16_t disp_buf[ST7789_WIDTH * HOR_LEN];
//
//static void ST7789_WriteCommand(uint8_t cmd)
//{
//	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET);
//	HAL_SPI_Transmit(&hspi2, &cmd, sizeof(cmd), HAL_MAX_DELAY);
//	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
//}
//
//static void ST7789_WriteData(uint8_t *buff, size_t buff_size)
//{
//	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
//
//	// split data in small chunks because HAL can't send more than 64K at once
//
//	while (buff_size > 0) {
//		uint16_t chunk_size = buff_size > 65535 ? 65535 : buff_size;
//		if (DMA_MIN_SIZE <= buff_size)
//		{
//			HAL_SPI_Transmit_DMA(&hspi2, buff, chunk_size);
//			while (ST7789_SPI_PORT.hdmatx->State != HAL_DMA_STATE_READY)
//			{}
//		}
//		else
//			HAL_SPI_Transmit(&hspi2, buff, chunk_size, HAL_MAX_DELAY);
//		buff += chunk_size;
//		buff_size -= chunk_size;
//	}
//
//	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
//}
//
//static void ST7789_WriteSmallData(uint8_t data)
//{
//	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
//	HAL_SPI_Transmit(&hspi2, &data, sizeof(data), HAL_MAX_DELAY);
//	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
//}
//
//void ST7789_SetRotation(uint8_t m)
//{
//	ST7789_WriteCommand(ST7789_MADCTL);	// MADCTL
//	switch (m) {
//	case 0:
//		ST7789_WriteSmallData(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
//		break;
//	case 1:
//		ST7789_WriteSmallData(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
//		break;
//	case 2:
//		ST7789_WriteSmallData(ST7789_MADCTL_RGB);
//		break;
//	case 3:
//		ST7789_WriteSmallData(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
//		break;
//	default:
//		break;
//	}
//}
//
//static void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
//{
//	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
//	uint16_t x_start = x0 + X_SHIFT, x_end = x1 + X_SHIFT;
//	uint16_t y_start = y0 + Y_SHIFT, y_end = y1 + Y_SHIFT;
//
//	/* Column Address set */
//	ST7789_WriteCommand(ST7789_CASET);
//	{
//		uint8_t data[] = {x_start >> 8, x_start & 0xFF, x_end >> 8, x_end & 0xFF};
//		ST7789_WriteData(data, sizeof(data));
//	}
//
//	/* Row Address set */
//	ST7789_WriteCommand(ST7789_RASET);
//	{
//		uint8_t data[] = {y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF};
//		ST7789_WriteData(data, sizeof(data));
//	}
//	/* Write to RAM */
//	ST7789_WriteCommand(ST7789_RAMWR);
//	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
//}
//
//void ST7789_Init(void)
//{
//	memset(disp_buf, 0, sizeof(disp_buf));
//
//	HAL_Delay(100);
//	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
//    HAL_Delay(100);
//    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
//    HAL_Delay(100);
//
//    ST7789_WriteCommand(ST7789_COLMOD);		//	Set color mode
//    ST7789_WriteSmallData(ST7789_COLOR_MODE_16bit);
//  	ST7789_WriteCommand(0xB2);				//	Porch control
//	{
//		uint8_t data[] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
//		ST7789_WriteData(data, sizeof(data));
//	}
//	ST7789_SetRotation(ST7789_ROTATION);	//	MADCTL (Display Rotation)
//
//	/* Internal LCD Voltage generator settings */
//    ST7789_WriteCommand(0XB7);				//	Gate Control
//    ST7789_WriteSmallData(0x35);			//	Default value
//    ST7789_WriteCommand(0xBB);				//	VCOM setting
//    ST7789_WriteSmallData(0x19);			//	0.725v (default 0.75v for 0x20)
//    ST7789_WriteCommand(0xC0);				//	LCMCTRL
//    ST7789_WriteSmallData (0x2C);			//	Default value
//    ST7789_WriteCommand (0xC2);				//	VDV and VRH command Enable
//    ST7789_WriteSmallData (0x01);			//	Default value
//    ST7789_WriteCommand (0xC3);				//	VRH set
//    ST7789_WriteSmallData (0x12);			//	+-4.45v (defalut +-4.1v for 0x0B)
//    ST7789_WriteCommand (0xC4);				//	VDV set
//    ST7789_WriteSmallData (0x20);			//	Default value
//    ST7789_WriteCommand (0xC6);				//	Frame rate control in normal mode
//    ST7789_WriteSmallData (0x0F);			//	Default value (60HZ)
//    ST7789_WriteCommand (0xD0);				//	Power control
//    ST7789_WriteSmallData (0xA4);			//	Default value
//    ST7789_WriteSmallData (0xA1);			//	Default value
//	/**************** Division line ****************/
//
//	ST7789_WriteCommand(0xE0);
//	{
//		uint8_t data[] = {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23};
//		ST7789_WriteData(data, sizeof(data));
//	}
//
//    ST7789_WriteCommand(0xE1);
//	{
//		uint8_t data[] = {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23};
//		ST7789_WriteData(data, sizeof(data));
//	}
//    ST7789_WriteCommand (ST7789_INVON);		//	Inversion ON
//	ST7789_WriteCommand (ST7789_SLPOUT);	//	Out of sleep mode
//  	ST7789_WriteCommand (ST7789_NORON);		//	Normal Display on
//  	ST7789_WriteCommand (ST7789_DISPON);	//	Main screen turned on
//
//	HAL_Delay(50);
//	ST7789_Fill_Color(RED);				//	Fill with Black.
//}
//
//void ST7789_Fill_Color(uint16_t color)
//{
//	uint16_t i;
//	ST7789_SetAddressWindow(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
//	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
//
//	for (i = 0; i < ST7789_HEIGHT / HOR_LEN; i++)
//	{
//		memset(disp_buf, color, sizeof(disp_buf));
//		ST7789_WriteData(disp_buf, sizeof(disp_buf));
//	}
//
//	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
//}
//
//void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
//{
//	if ((x < 0) || (x >= ST7789_WIDTH) ||
//		 (y < 0) || (y >= ST7789_HEIGHT))	return;
//
//	ST7789_SetAddressWindow(x, y, x, y);
//	uint8_t data[] = {color >> 8, color & 0xFF};
//	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
//	ST7789_WriteData(data, sizeof(data));
//	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
//}
//
//void ST7789_Fill(uint16_t xSta, uint16_t ySta, uint16_t xEnd, uint16_t yEnd, uint16_t color)
//{
//	if ((xEnd < 0) || (xEnd >= ST7789_WIDTH) ||
//		 (yEnd < 0) || (yEnd >= ST7789_HEIGHT))	return;
//	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
//	uint16_t i, j;
//	ST7789_SetAddressWindow(xSta, ySta, xEnd, yEnd);
//	for (i = ySta; i <= yEnd; i++)
//		for (j = xSta; j <= xEnd; j++) {
//			uint8_t data[] = {color >> 8, color & 0xFF};
//			ST7789_WriteData(data, sizeof(data));
//		}
//	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
//}
//
//void ST7789_DrawPixel_4px(uint16_t x, uint16_t y, uint16_t color)
//{
//	if ((x <= 0) || (x > ST7789_WIDTH) ||
//		 (y <= 0) || (y > ST7789_HEIGHT))	return;
//	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
//	ST7789_Fill(x - 1, y - 1, x + 1, y + 1, color);
//	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
//}
//
//void ST7789_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
//	uint16_t swap;
//    uint16_t steep = ABS(y1 - y0) > ABS(x1 - x0);
//    if (steep) {
//		swap = x0;
//		x0 = y0;
//		y0 = swap;
//
//		swap = x1;
//		x1 = y1;
//		y1 = swap;
//        //_swap_int16_t(x0, y0);
//        //_swap_int16_t(x1, y1);
//    }
//
//    if (x0 > x1) {
//		swap = x0;
//		x0 = x1;
//		x1 = swap;
//
//		swap = y0;
//		y0 = y1;
//		y1 = swap;
//        //_swap_int16_t(x0, x1);
//        //_swap_int16_t(y0, y1);
//    }
//
//    int16_t dx, dy;
//    dx = x1 - x0;
//    dy = ABS(y1 - y0);
//
//    int16_t err = dx / 2;
//    int16_t ystep;
//
//    if (y0 < y1) {
//        ystep = 1;
//    } else {
//        ystep = -1;
//    }
//
//    for (; x0<=x1; x0++) {
//        if (steep) {
//            ST7789_DrawPixel(y0, x0, color);
//        } else {
//            ST7789_DrawPixel(x0, y0, color);
//        }
//        err -= dy;
//        if (err < 0) {
//            y0 += ystep;
//            err += dx;
//        }
//    }
//}
//
//void ST7789_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
//{
//	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
//	ST7789_DrawLine(x1, y1, x2, y1, color);
//	ST7789_DrawLine(x1, y1, x1, y2, color);
//	ST7789_DrawLine(x1, y2, x2, y2, color);
//	ST7789_DrawLine(x2, y1, x2, y2, color);
//	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
//}
//
//






