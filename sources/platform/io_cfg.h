/**
 ******************************************************************************
 * @author: JiaHui
 * @date:   --/--/----
 ******************************************************************************
**/
#ifndef __IO_CFG_H__
#define __IO_CFG_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "system_stm32f10x.h"
#include "core_cm3.h"
#include <stdint.h>
#include <stdbool.h>


/******************************************************************************
 *Pin map button
*******************************************************************************/
#define BUTTON_LEFT_DOWN_PIN		    GPIO_Pin_15
#define BUTTON_LEFT_DOWN_PORT		    GPIOB
#define BUTTON_LEFT_DOWN_CLOCK			RCC_APB2Periph_GPIOB

#define BUTTON_LEFT_UP_PIN			    GPIO_Pin_4
#define BUTTON_LEFT_UP_PORT		        GPIOB
#define BUTTON_LEFT_UP_CLOCK		    RCC_APB2Periph_GPIOB

#define BUTTON_RIGHT_UP_PIN			    GPIO_Pin_15
#define BUTTON_RIGHT_UP_PORT		    GPIOA
#define BUTTON_RIGHT_UP_CLOCK		    RCC_APB2Periph_GPIOA

#define BUTTON_RIGHT_DOWN_PIN		    GPIO_Pin_0
#define BUTTON_RIGHT_DOWN_PORT			GPIOA
#define BUTTON_RIGHT_DOWN_CLOCK			RCC_APB2Periph_GPIOA


//Those keys can simultaneously occur
#define	BUTTON_RIGHT_DOWN_PRESSED 	    1
#define	BUTTON_RIGHT_UP_PRESSED 	    2
#define	BUTTON_LEFT_DOWN_PRESSED 	    4
#define	BUTTON_LEFT_UP_PRESSED 		    8

//Those keys can not simultaneously occur
#define	BUTTON_RIGHT_DOWN_LONG_PRESSED  9
#define	BUTTON_RIGHT_UP_LONG_PRESSED    0
#define	BUTTON_LEFT_DOWN_LONG_PRESSED   11
#define	BUTTON_LEFT_UP_LONG_PRESSED     12


/******************************************************************************
 *Pin map Oled SH1106 <- SPI1
*******************************************************************************/
#define	SH1106_DCX_PIN			        GPIO_Pin_4
#define	SH1106_DCX_PORT				    GPIOA
#define	SH1106_DCX_CLOCK			    RCC_APB2Periph_GPIOA

#define	SH1106_SCK_PIN 				    GPIO_Pin_5
#define	SH1106_SCK_PORT				    GPIOA
#define	SH1106_SCK_CLOCK			    RCC_APB2Periph_GPIOA

#define	SH1106_RST_PIN 				    GPIO_Pin_6
#define	SH1106_RST_PORT				    GPIOA
#define	SH1106_RST_CLOCK			    RCC_APB2Periph_GPIOA

#define	SH1106_MOSI_PIN 			    GPIO_Pin_7
#define	SH1106_MOSI_PORT			    GPIOA
#define	SH1106_MOSI_CLOCK			    RCC_APB2Periph_GPIOA

/******************************************************************************
 *Pin map BLE <- UART1
*******************************************************************************/
#define	BLE_EN_PIN 				        GPIO_Pin_8
#define	BLE_EN_PORT					    GPIOA
#define	BLE_EN_CLOCK				    RCC_APB2Periph_GPIOA

#define	BLE_RX_PIN  				    GPIO_Pin_10
#define	BLE_RX_PORT					    GPIOA
#define	BLE_RX_CLOCK				    RCC_APB2Periph_GPIOA


#define	BLE_TX_PIN  				    GPIO_Pin_9
#define	BLE_TX_PORT					    GPIOA
#define	BLE_TX_CLOCK			        RCC_APB2Periph_GPIOA

#define	BLE_STT_PIN					    GPIO_Pin_11
#define	BLE_STT_PORT				    GPIOA
#define	BLE_STT_CLOCK			        RCC_APB2Periph_GPIOA

#define	BLE_RST_PIN 				    GPIO_Pin_3
#define BLE_RST_PORT				    GPIOB
#define BLE_RST_CLOCK				    RCC_APB2Periph_GPIOB

/******************************************************************************
 *Pin map MPU6050, Compass, humid sensor <- I2C1
*******************************************************************************/
#define	I2C_SCL_PIN					    GPIO_Pin_6
#define I2C_SCL_PORT				    GPIOB
#define I2C_SCL_CLOCK				    RCC_APB2Periph_GPIOB

#define	I2C_SDA_PIN				        GPIO_Pin_7
#define I2C_SDA_PORT				    GPIOB
#define I2C_SDA_CLOCK				    RCC_APB2Periph_GPIOB

/******************************************************************************
 *Pin map power pin
*******************************************************************************/
#define	PWR_EN_PIN					    GPIO_Pin_0
#define PWR_EN_PORT					    GPIOB
#define PWR_EN_CLOCK				    RCC_APB2Periph_GPIOB


/******************************************************************************
 *Config value
*******************************************************************************/
typedef struct{
    uint8_t screen_brightness : 8;		    /* 0 - 255*/
	uint8_t screen_time : 5;			/* 0 - 31*/	
	bool 	screen_inverse : 1;				/* 0 - 1*/
}watch_config;

/******************************************************************************
 *Methods
*******************************************************************************/
extern void btn_ul_init(void);
extern void btn_ur_init(void);
extern void btn_dl_init(void);
extern void btn_dr_init(void);

extern uint8_t btn_ul_read();
extern uint8_t btn_ur_read();
extern uint8_t btn_dl_read();
extern uint8_t btn_dr_read();

extern void oled_hw_init(void);
extern void ble_hw_init(void);
extern void power_pin_init(void);
extern void power_pin_on(void);
extern void power_pin_off(void);

extern void config_data_init(void);
extern void config_data_read(watch_config* cfg);
extern void config_data_write(watch_config* cfg);

extern void watchdog_hw_init(void);
extern void watchdog_hw_reload(void);


/**
 *  I2C
 */
extern void i2c_hw_init(void);

extern int16_t i2c_hw_start(uint8_t address, uint8_t direction, uint8_t ack);
extern uint8_t i2c_hw_stop(void);
extern uint8_t i2c_hw_read_ack(void);
extern uint8_t i2c_hw_read_nack(void);

extern void i2c_hw_write(uint8_t address, uint8_t reg, uint8_t data);
extern uint8_t i2c_hw_read(uint8_t address, uint8_t reg);

extern void i2c_hw_write_multi(uint8_t address, uint8_t reg, uint8_t* data, uint16_t count);
extern void i2c_hw_read_multi(uint8_t address, uint8_t reg, uint8_t* data, uint16_t count);

/**
 *  ADC - Battery
 */
extern void adc_hw_init(void);
extern float adc_get_bat_voltage(void);
extern float adc_get_temp(void);

#ifdef __cplusplus
}
#endif

#endif //__IO_CFG_H__