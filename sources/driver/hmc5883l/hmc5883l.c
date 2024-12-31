#include "hmc5883l.h"
#include "io_cfg.h"
#include "math.h"

/******************************???ò???*************************************/
unsigned char OSR = 0;		//Over Sample Ratio
													//00 = 512; 01 = 256; 10 = 128; 11 = 64
unsigned char ODR = 1;		//Typical Data Output Rate (Hz) 
													//00	10Hz
													//01	50Hz
													//10	100Hz
													//11	200Hz
unsigned char RNG = 0;		//Full Scale
													//00	2G
													//01	8G
unsigned char MOD = 0;		//Operating Mode 
													//00 = Standby
													//01 = Continuous(Default).
/*****************************??????************************************/											
int mag_x_offset = 0, mag_y_offset = 0, mag_z_offset = 0;		//?????????
float mag_y_scale = 1, mag_z_scale = 1;											//??????????
/*************************************************************************/





static void HMC5883L_Write_Byte(unsigned char addr, unsigned char dat)
{
	i2c_hw_write(HMC5883_ADDRESS, addr, dat);
}

unsigned char HMC5883L_Read_Byte(unsigned char addr)
{
	return i2c_hw_read(HMC5883_ADDRESS, addr);
}


void HMC5883LRead(HMC5883L_t * DataStruct)
{
	uint8_t temp[6];
	i2c_hw_read_multi(HMC5883_ADDRESS, HMC5883_REG_OUTXM, temp, 6);
	DataStruct->x = (int16_t)(temp[0] << 8 | temp[1]) * 0.92f;
	DataStruct->z = (int16_t)(temp[2] << 8 | temp[3]) * 0.92f;
	DataStruct->y = (int16_t)(temp[4] << 8 | temp[5]) * 0.92f;
}

unsigned char HMC5883LInit(void)
{
	if(HMC5883L_Read_Byte(HMC5883_REG_IDA) == 0x48)
	{
		HMC5883L_Write_Byte(HMC5883_REG_CRA, 0x10); //reg A
		HMC5883L_Write_Byte(HMC5883_REG_CRB, 0x20);	//reg B    /* Gan = 1090; 0.92mG */
		HMC5883L_Write_Byte(HMC5883_REG_MODE, 0x00);	//Continuos mode
		return 1;
	}
	else return 0;
}
float HMC5883LCalculateHeading(HMC5883L_t *data)
{
	float heading;
	heading = (float)atan2(data->x * -1, data->y * -1) * 180 / 3.14159265358f + 180;
	heading += 90;
	if(heading > 360) heading -= 360;
	return heading;
}