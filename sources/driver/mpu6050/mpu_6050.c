/*
 * mpu6050.c
 *
 *  Created on: Nov 13, 2019
 *      Author: Bulanov Konstantin
 *
 *  Contact information
 *  -------------------
 *
 * e-mail   :  leech001@gmail.com
 */

/*
 * |---------------------------------------------------------------------------------
 * | Copyright (C) Bulanov Konstantin,2021
 * |
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * |
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |
 * | Kalman filter algorithm used from https://github.com/TKJElectronics/KalmanFilter
 * |---------------------------------------------------------------------------------
 */

#include <math.h>
#include "mpu_6050.h"
#include "io_cfg.h"
#include "os_task.h"

#define RAD_TO_DEG 57.295779513082320876798154814105

#define WHO_AM_I_REG 			0x75
#define CONFIG 						0x1A
#define INT_ENABLE 				0x38
#define MOT_THR 					0x1F
#define MOT_DUR 					0x69
#define INT_PIN_CFG 			0x37
#define PWR_MGMT_1_REG 		0x6B
#define PWR_MGMT_2_REG 		0x6C
#define SMPLRT_DIV_REG 		0x19
#define ACCEL_CONFIG_REG 	0x1C
#define ACCEL_XOUT_H_REG 	0x3B
#define TEMP_OUT_H_REG 		0x41
#define GYRO_CONFIG_REG 	0x1B
#define GYRO_XOUT_H_REG 	0x43

// Setup MPU6050
#define MPU6050_ADDR 0xD0
const uint16_t i2c_timeout = 100;
const double Accel_Z_corrector = 14418.0;
uint32_t timer;
Kalman_t KalmanX = {
    .Q_angle = 0.001f,
    .Q_bias = 0.003f,
    .R_measure = 0.03f
};

Kalman_t KalmanY = {
    .Q_angle = 0.001f,
    .Q_bias = 0.003f,
    .R_measure = 0.03f,
};


bool MPU6050_Init(void)
{
	if(i2c_hw_read(MPU6050_ADDR, WHO_AM_I_REG) == 0x68)
	{
		i2c_hw_write(MPU6050_ADDR, PWR_MGMT_1_REG, 0x00);			
		i2c_hw_write(MPU6050_ADDR, SMPLRT_DIV_REG, 0x07);		//Data rate 1kHz where Gyroscope Output Rate = 8kHz
		i2c_hw_write(MPU6050_ADDR, GYRO_CONFIG_REG, 0x00);		//Full scale range
		i2c_hw_write(MPU6050_ADDR, ACCEL_CONFIG_REG, 0x00);	    //Full scale range
		return true;
	}
	else return false;
}
void MPU6050_LowPwrAccelOnly(void)
{
	uint8_t temp = i2c_hw_read(MPU6050_ADDR, PWR_MGMT_1_REG);
	i2c_hw_write(MPU6050_ADDR, PWR_MGMT_1_REG, temp & ~0x30);	//Clear Sleep and cycle
	i2c_hw_write(MPU6050_ADDR, PWR_MGMT_1_REG, temp |  0x00);	//Make sure MPU is running
	
	
  temp = i2c_hw_read(MPU6050_ADDR, PWR_MGMT_2_REG);
  i2c_hw_write(MPU6050_ADDR, PWR_MGMT_2_REG, temp & ~0x38); // Clear standby XA, YA, and ZA bits [3:5]
  i2c_hw_write(MPU6050_ADDR, PWR_MGMT_2_REG, temp |  0x00); // Set XA, YA, and ZA bits [3:5] to zero to make sure accelerometer is running
	
	temp = i2c_hw_read(MPU6050_ADDR, CONFIG);
  i2c_hw_write(MPU6050_ADDR, CONFIG, temp & ~0x07); // Clear Filter [2:0]
  i2c_hw_write(MPU6050_ADDR, CONFIG, temp |  0x00); // Set filter DLPF_CFG = 0
	
// 	temp = i2c_hw_read(MPU6050_ADDR, INT_ENABLE);
//   i2c_hw_write(MPU6050_ADDR, INT_ENABLE, temp & ~0xFF); // Clear all interrupts
//   i2c_hw_write(MPU6050_ADDR, INT_ENABLE, temp |  0x40); // MOT_EN
	
// 	i2c_hw_write(MPU6050_ADDR, MOT_THR, 0x80); // Set motion detection to 0.256 g; LSB = 2 mg
//   i2c_hw_write(MPU6050_ADDR, MOT_DUR, 0x01); // Set motion detect duration to 1  ms; LSB is 1 ms @ 1 kHz rate
	
	
	temp = i2c_hw_read(MPU6050_ADDR, ACCEL_CONFIG_REG);
  i2c_hw_write(MPU6050_ADDR, ACCEL_CONFIG_REG, temp & ~0x07); // Clear high-pass filter bits [2:0]
  i2c_hw_write(MPU6050_ADDR, ACCEL_CONFIG_REG, temp |  0x07); // Set ACCEL_HPF to 7; hold the initial accleration value as a referance
	
	temp = i2c_hw_read(MPU6050_ADDR, PWR_MGMT_2_REG);
  i2c_hw_write(MPU6050_ADDR, PWR_MGMT_2_REG, temp & ~0xC7); // Clear standby XA, YA, and ZA bits [3:5] and LP_WAKE_CTRL bits [6:7]
  i2c_hw_write(MPU6050_ADDR, PWR_MGMT_2_REG, temp |  0x47); // Set wakeup frequency to 5 Hz, and disable XG, YG, and ZG gyros (bits [0:2])  
	
	temp = i2c_hw_read(MPU6050_ADDR, PWR_MGMT_1_REG);
  i2c_hw_write(MPU6050_ADDR, PWR_MGMT_1_REG, temp & ~0x20); // Clear sleep and cycle bit 5
  i2c_hw_write(MPU6050_ADDR, PWR_MGMT_1_REG, temp |  0x20); // Set cycle bit 5 to begin low power accelerometer motion interrupts
	
	//i2c_hw_write(MPU6050_ADDR, INT_PIN_CFG, 0x80); // Clear sleep and cycle bit 5
}
void MPU6050_Read_Accel(MPU6050_t * DataStruct)
{
	uint8_t Rec_Data[6];
	i2c_hw_read_multi(MPU6050_ADDR, ACCEL_XOUT_H_REG, Rec_Data, 6);
  DataStruct->Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
  DataStruct->Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
  DataStruct->Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);
  /*** convert the RAW values into acceleration in 'g'
       we have to divide according to the Full scale value set in FS_SEL
       I have configured FS_SEL = 0. So I am dividing by 16384.0
       for more details check ACCEL_CONFIG Register              ****/

  DataStruct->Ax = DataStruct->Accel_X_RAW / 16384.0;
  DataStruct->Ay = DataStruct->Accel_Y_RAW / 16384.0;
  DataStruct->Az = DataStruct->Accel_Z_RAW / Accel_Z_corrector;
}
void MPU6050_Read_Gyro(MPU6050_t * DataStruct)
{
	uint8_t Rec_Data[6];
	i2c_hw_read_multi(MPU6050_ADDR, GYRO_XOUT_H_REG, Rec_Data, 6);
  DataStruct->Gyro_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
  DataStruct->Gyro_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
  DataStruct->Gyro_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);
  /*** convert the RAW values into dps (?/s)
       we have to divide according to the Full scale value set in FS_SEL
       I have configured FS_SEL = 0. So I am dividing by 131.0
       for more details check GYRO_CONFIG Register              ****/

  DataStruct->Gx = (DataStruct->Gyro_X_RAW / 131.0) ;
  DataStruct->Gy = (DataStruct->Gyro_Y_RAW / 131.0) ;
  DataStruct->Gz = -1 * DataStruct->Gyro_Z_RAW / 131.0;
}
void MPU6050_Read_Temp(MPU6050_t *DataStruct)
{
    uint8_t Rec_Data[2];
    int16_t temp;
    // Read 2 BYTES of data starting from TEMP_OUT_H_REG register
		i2c_hw_read_multi(MPU6050_ADDR, TEMP_OUT_H_REG, Rec_Data, 2);

    temp = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
    DataStruct->Temperature = (float)((int16_t)temp / (float)340.0 + (float)36.53);
}
void MPU6050_Read_Angle(MPU6050_t *DataStruct)
{		
    MPU6050_Read_Accel(DataStruct);
    MPU6050_Read_Gyro(DataStruct);
    // Kalman angle solve
    double dt = (double)(os_task_get_tick() - timer) / 1000;
    timer = os_task_get_tick();
    double roll;
    double roll_sqrt = sqrt(DataStruct->Accel_X_RAW * DataStruct->Accel_X_RAW + DataStruct->Accel_Z_RAW * DataStruct->Accel_Z_RAW);
    if (roll_sqrt != 0.0)
    {
        roll = atan(DataStruct->Accel_Y_RAW / roll_sqrt) * RAD_TO_DEG;
    }
    else
    {
        roll = 0.0;
    }
    double pitch = atan2(DataStruct->Accel_X_RAW, DataStruct->Accel_Z_RAW) * RAD_TO_DEG;

    DataStruct->KalmanAngleY = Kalman_getAngle(&KalmanY, pitch, DataStruct->Gy, dt);

    DataStruct->KalmanAngleX = Kalman_getAngle(&KalmanX, roll, DataStruct->Gx, dt);		
}
double Kalman_getAngle(Kalman_t *Kalman, double newAngle, double newRate, double dt)
{
    double rate = newRate - Kalman->bias;
    Kalman->angle += dt * rate;

    Kalman->P[0][0] += dt * (dt * Kalman->P[1][1] - Kalman->P[0][1] - Kalman->P[1][0] + Kalman->Q_angle);
    Kalman->P[0][1] -= dt * Kalman->P[1][1];
    Kalman->P[1][0] -= dt * Kalman->P[1][1];
    Kalman->P[1][1] += Kalman->Q_bias * dt;

    double S = Kalman->P[0][0] + Kalman->R_measure;
    double K[2];
    K[0] = Kalman->P[0][0] / S;
    K[1] = Kalman->P[1][0] / S;

    double y = newAngle - Kalman->angle;
    Kalman->angle += K[0] * y;
    Kalman->bias += K[1] * y;

    double P00_temp = Kalman->P[0][0];
    double P01_temp = Kalman->P[0][1];

    Kalman->P[0][0] -= K[0] * P00_temp;
    Kalman->P[0][1] -= K[0] * P01_temp;
    Kalman->P[1][0] -= K[1] * P00_temp;
    Kalman->P[1][1] -= K[1] * P01_temp;
    return Kalman->angle;
};