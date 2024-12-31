/**
 ******************************************************************************
 * @author: JiaHui
 * @date:   --/--/----
 ******************************************************************************
**/
#include <stdint.h>
#include <stdbool.h>
#include "io_cfg.h"


void btn_ul_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(BUTTON_LEFT_UP_CLOCK , ENABLE);
	GPIO_InitStructure.GPIO_Pin   = BUTTON_LEFT_UP_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(BUTTON_LEFT_UP_PORT, &GPIO_InitStructure);	
}
void btn_ur_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(BUTTON_RIGHT_UP_CLOCK , ENABLE);
	GPIO_InitStructure.GPIO_Pin   = BUTTON_RIGHT_UP_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(BUTTON_RIGHT_UP_PORT, &GPIO_InitStructure);	
}
void btn_dl_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(BUTTON_LEFT_DOWN_CLOCK , ENABLE);
	GPIO_InitStructure.GPIO_Pin   = BUTTON_LEFT_DOWN_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(BUTTON_LEFT_DOWN_PORT, &GPIO_InitStructure);	
}
void btn_dr_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(BUTTON_RIGHT_DOWN_CLOCK , ENABLE);
	GPIO_InitStructure.GPIO_Pin   = BUTTON_RIGHT_DOWN_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(BUTTON_RIGHT_DOWN_PORT, &GPIO_InitStructure);	
}

uint8_t btn_ul_read() {
	return  GPIO_ReadInputDataBit(BUTTON_LEFT_UP_PORT, BUTTON_LEFT_UP_PIN);
}

uint8_t btn_ur_read() {
	return  GPIO_ReadInputDataBit(BUTTON_RIGHT_UP_PORT, BUTTON_RIGHT_UP_PIN);
}

uint8_t btn_dl_read() {
	return  GPIO_ReadInputDataBit(BUTTON_LEFT_DOWN_PORT, BUTTON_LEFT_DOWN_PIN);
}
uint8_t btn_dr_read() {
	return  GPIO_ReadInputDataBit(BUTTON_RIGHT_DOWN_PORT, BUTTON_RIGHT_DOWN_PIN);
}


void oled_hw_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB2PeriphClockCmd(SH1106_DCX_CLOCK, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = SH1106_DCX_PIN; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SH1106_DCX_PORT, &GPIO_InitStruct);	
	
	GPIO_InitStruct.GPIO_Pin = SH1106_RST_PIN; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SH1106_RST_PORT, &GPIO_InitStruct);	
	
	GPIO_InitStruct.GPIO_Pin = SH1106_SCK_PIN ; 	
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(SH1106_SCK_PORT, &GPIO_InitStruct);	
	GPIO_PinRemapConfig(GPIO_Remap_SPI1, DISABLE); //Remap = 0	
	
	GPIO_InitStruct.GPIO_Pin = SH1106_MOSI_PIN; 	
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(SH1106_MOSI_PORT, &GPIO_InitStruct);	
	GPIO_PinRemapConfig(GPIO_Remap_SPI1, DISABLE); //Remap = 0	
	
	//SPI1 Init
	SPI_InitTypeDef SPI_InitStruct;
	SPI_InitStruct.SPI_Direction 			= SPI_Direction_Tx;
	SPI_InitStruct.SPI_Mode 				= SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize 			= SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL					= SPI_CPOL_Low;			// 	CPOL = 0
	SPI_InitStruct.SPI_CPHA					= SPI_CPHA_1Edge;		//	CPHA = 1
	SPI_InitStruct.SPI_NSS					= SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler	= SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_FirstBit				= SPI_FirstBit_MSB;	

	SPI_Init(SPI1, &SPI_InitStruct);
	SPI_Cmd(SPI1, ENABLE);


	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_InitTypeDef DMA_InitStruct;

	DMA_InitStruct.DMA_PeripheralBaseAddr = 0;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStruct.DMA_BufferSize = 0;
	DMA_InitStruct.DMA_MemoryBaseAddr = 0;			 		
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStruct.DMA_Mode	= DMA_Mode_Normal;
	DMA_InitStruct.DMA_Priority	= DMA_Priority_Low;
	DMA_InitStruct.DMA_M2M	= DMA_M2M_Disable;
	DMA_Init(DMA1_Channel3, &DMA_InitStruct);
}
void ble_hw_init(void)
{

}

void power_pin_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(PWR_EN_CLOCK , ENABLE);
	GPIO_InitStructure.GPIO_Pin   = PWR_EN_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(PWR_EN_PORT, &GPIO_InitStructure);	
}
void power_pin_on(void)
{	
	GPIO_SetBits(PWR_EN_PORT, PWR_EN_PIN);
}
void power_pin_off(void)
{
	GPIO_ResetBits(PWR_EN_PORT, PWR_EN_PIN);
}




void config_data_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
}
void config_data_read(watch_config* cfg)
{
	// cfg->screen_time = BKP_ReadBackupRegister(BKP_DR2);
	// cfg->screen_brightness = BKP_ReadBackupRegister(BKP_DR3);
	// cfg->screen_inverse = BKP_ReadBackupRegister(BKP_DR4);
	uint16_t value = BKP_ReadBackupRegister(BKP_DR2);
	cfg->screen_brightness = (value & 0x00ff);
	cfg->screen_time = (value >> 8) & 0b00011111;
	cfg->screen_inverse = (value >> 13) & 0x01;

	cfg->screen_brightness = (cfg->screen_brightness > 40) ? cfg->screen_brightness : 40;
	cfg->screen_time = (cfg->screen_time > 5) ? cfg->screen_time : 5;
}
void config_data_write(watch_config* cfg)
{
	PWR_BackupAccessCmd(ENABLE);
	uint16_t value = 0x0000 | (cfg->screen_brightness) | (cfg->screen_time << 8) | (cfg->screen_inverse << 13);
	BKP_WriteBackupRegister(BKP_DR2, (uint16_t)value);
	PWR_BackupAccessCmd(DISABLE);
}

void watchdog_hw_init(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	while(IWDG_GetFlagStatus(IWDG_FLAG_PVU));
	IWDG_SetPrescaler(IWDG_Prescaler_32);				//Min timeout 0.8ms, max time out 3276.8ms, LSI 40kHz/32 = 1.25kHz = 0.8ms
	while(IWDG_GetFlagStatus(IWDG_FLAG_RVU));
	IWDG_SetReload(0x04E2);								//1250 cycles * 0.8 = 1000ms -> period
	IWDG_Enable();
}
void watchdog_hw_reload(void)
{
	IWDG_ReloadCounter();
}

void i2c_hw_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;		// For I2C
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	I2C_InitTypeDef I2C_InitStruct;
	//Config the I2C
	//Set the clock speed of I2C. It has to be equal with the external device
	I2C_InitStruct.I2C_ClockSpeed = 400000;
	//I2C mode
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;	//Fast mode
	//I2C Acknowladge configuration
	I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	//Enable the I2C with the prepared configuration
	I2C_Init(I2C1, &I2C_InitStruct);
	//And start the I2C 
	I2C_Cmd(I2C1, ENABLE);
}

#define	TIME_OUT 0xFFFF

int16_t i2c_hw_start(uint8_t address, uint8_t direction, uint8_t ack)
{
	I2C_GenerateSTART(I2C1, ENABLE);
	uint32_t I2C_Timeout = TIME_OUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) //EV5
	{
		if (--I2C_Timeout == 0x00) 
		{
			return 0;
		}
	}
	I2C_AcknowledgeConfig(I2C1, ack);	
	
	if(direction == I2C_Direction_Transmitter)
	{
		I2C_Send7bitAddress(I2C1, address, I2C_Direction_Transmitter);
		I2C_Timeout = TIME_OUT;
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) //Addr mismatch EV6
		{
			if (--I2C_Timeout == 0x00) 
			{
				return 0;
			}
		}	
	}
	else if(direction == I2C_Direction_Receiver)
	{ 
		I2C_Send7bitAddress(I2C1, address, I2C_Direction_Receiver);
		I2C_Timeout = TIME_OUT;
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) //Addr mismatch EV6
		{
			if (--I2C_Timeout == 0x00) 
			{
				return 0;
			}
		}	
	}
	I2C_ClearFlag(I2C1, I2C_FLAG_ADDR); // Clear ADDR flag
	return 1;	
}

uint8_t i2c_hw_stop(void) {	
	uint16_t I2C_Timeout = TIME_OUT;
	while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE) || !I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF)) //Wait Tx empty and byte transmitted
	{
		if (--I2C_Timeout == 0x00) 
		{
			return 0;
		}
	}
	/* Generate stop */
	I2C_GenerateSTOP(I2C1, ENABLE);
	return 1;
}
uint8_t i2c_hw_read_ack(void) {
	uint8_t data;	
	/* Enable ACK */
	I2C_AcknowledgeConfig(I2C1, ENABLE);	
	/* Wait till not received */
	uint16_t I2C_Timeout = TIME_OUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) 
	{
		if (--I2C_Timeout == 0x00) return 0;
	}	
	/* Read data */
	data = I2C_ReceiveData(I2C1);	
	/* Return data */
	return data;
}
uint8_t i2c_hw_read_nack() {
	uint8_t data;	
	/* Disable ACK */
	I2C_AcknowledgeConfig(I2C1, DISABLE);	
	/* Generate stop */
	I2C_GenerateSTOP(I2C1, ENABLE);	
	/* Wait till received */
	uint16_t I2C_Timeout = TIME_OUT;
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) 
	{
		if (--I2C_Timeout == 0x00) return 0;
	}	
	/* Read data */
	data = I2C_ReceiveData(I2C1);	
	/* Return data */
	return data;
}


static void i2c_hw_write_data(uint8_t data)
{
	uint16_t I2C_Timeout = TIME_OUT;
	while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE) && I2C_Timeout) {
		I2C_Timeout--;
	}	
	/* Send I2C data */
	I2C_SendData(I2C1, data);
}

void i2c_hw_write(uint8_t address, uint8_t reg, uint8_t data) {
	i2c_hw_start(address, I2C_Direction_Transmitter, DISABLE);
	i2c_hw_write_data(reg);
	i2c_hw_write_data(data);
	i2c_hw_stop();
}

uint8_t i2c_hw_read(uint8_t address, uint8_t reg) {
	uint8_t received_data;
	i2c_hw_start(address, I2C_Direction_Transmitter, ENABLE);
	i2c_hw_write_data(reg);	//EV8_1
	//i2c_hw_stop();
	i2c_hw_start(address, I2C_Direction_Receiver, DISABLE);
	received_data = i2c_hw_read_nack();
	return received_data;
}
void i2c_hw_write_multi(uint8_t address, uint8_t reg, uint8_t* data, uint16_t count) {
	i2c_hw_start(address, I2C_Direction_Transmitter, DISABLE);
	i2c_hw_write_data(reg);
	while (count--) {
		i2c_hw_write_data(*data++);
	}
	i2c_hw_stop();
}
void i2c_hw_read_multi(uint8_t address, uint8_t reg, uint8_t* data, uint16_t count) {
	i2c_hw_start(address, I2C_Direction_Transmitter, DISABLE);
	i2c_hw_write_data(reg);
	//i2c_hw_stop();
	//There is no need to stop acording to MPU6050 document
	i2c_hw_start(address, I2C_Direction_Receiver, ENABLE);
	while (count--) {
		if (!count) {
			/* Last byte */
			*data++ = i2c_hw_read_nack();
		} else {
			*data++ = i2c_hw_read_ack();
		}
	}
}


void adc_hw_init(void)
{
	RCC_ADCCLKConfig (RCC_PCLK2_Div6);
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_ADC1	, ENABLE);

	ADC_InitTypeDef ADC_InitStruct;
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStruct);
	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_Cmd(ADC1, ENABLE);

	//	ADC calibration (optional, but recommended at power on)
	ADC_ResetCalibration(ADC1);	// Reset previous calibration
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);	// Start new calibration (ADC must be off at that time)
	while(ADC_GetCalibrationStatus(ADC1));

	ADC_Cmd(ADC1, ENABLE);
	
}

float adc_get_bat_voltage(void)
{
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9 , 1, ADC_SampleTime_239Cycles5);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
	
	return (float)ADC_GetConversionValue(ADC1)*3.3*2 / 0xfff;
}
float adc_get_temp(void)
{
	ADC_RegularChannelConfig(ADC1, ADC_Channel_TempSensor, 1, ADC_SampleTime_239Cycles5); // define regular conversion config

	// start conversion
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	// start conversion (will be endless as we are in continuous mode)
	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

	uint16_t adc_value;
	float temperature;
#define V25 (uint16_t)(1750) // when V25=1.41V at ref 3.3V
#define Avg_Slope (uint16_t)(5)  //when avg_slope=4.3mV/C at ref 3.3V
	adc_value = ADC_GetConversionValue(ADC1);
	temperature = ((float)(V25-adc_value)/(float)Avg_Slope+25);
	return temperature;
}
