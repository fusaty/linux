/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __freax_SPI_CORGI_LCD_H
#define __freax_SPI_CORGI_LCD_H

#define CORGI_LCD_MODE_QVGA	1
#define CORGI_LCD_MODE_VGA	2

struct corgi_lcd_platform_data {
	int	init_mode;
	int	max_intensity;
	int	default_intensity;
	int	limit_mask;

	void (*notify)(int intensity);
	void (*kick_battery)(void);
};

void corgi_lcd_limit_intensity(int limit);

#endif /* __freax_SPI_CORGI_LCD_H */
