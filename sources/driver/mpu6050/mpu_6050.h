#ifndef __MPU_6050_H__
#define __MPU_6050_H__
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stdbool.h>

// MPU6050 structure
typedef struct
{
    int16_t Accel_X_RAW;
    int16_t Accel_Y_RAW;
    int16_t Accel_Z_RAW;
    double Ax;
    double Ay;
    double Az;
    int16_t Gyro_X_RAW;
    int16_t Gyro_Y_RAW;
    int16_t Gyro_Z_RAW;
    double Gx;
    double Gy;
    double Gz;
    float Temperature;

    double KalmanAngleX;
    double KalmanAngleY;
} MPU6050_t;

// Kalman structure
typedef struct
{
    double Q_angle;
    double Q_bias;
    double R_measure;
    double angle;
    double bias;
    double P[2][2];
} Kalman_t;


extern bool MPU6050_Init(void);
extern void MPU6050_LowPwrAccelOnly(void);
extern void MPU6050_Read_Accel(MPU6050_t *DataStruct);
extern void MPU6050_Read_Gyro(MPU6050_t *DataStruct);
extern void MPU6050_Read_Temp(MPU6050_t *DataStruct);
extern void MPU6050_Read_Angle(MPU6050_t *DataStruct);
extern double Kalman_getAngle(Kalman_t *Kalman, double newAngle, double newRate, double dt);


#ifdef __cplusplus
}
#endif
#endif