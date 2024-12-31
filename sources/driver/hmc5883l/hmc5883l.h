#ifndef _HMC5883L_H
#define _HMC5883L_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;

} HMC5883L_t;

/**
 * @brief iic address definition
 */
#define HMC5883_ADDRESS           0x3C        /**< iic address */
/**
 * @brief chip register definition
 */
#define HMC5883_REG_CRA          0x00      /**< cra register */
#define HMC5883_REG_CRB          0x01      /**< crb register */
#define HMC5883_REG_MODE         0x02      /**< mode register */

#define HMC5883_REG_OUTXM        0x03      /**< outxm register */
#define HMC5883_REG_OUTXL        0x04      /**< outxl register */

#define HMC5883_REG_OUTZM        0x05      /**< outzm register */
#define HMC5883_REG_OUTZL        0x06      /**< outzl register */

#define HMC5883_REG_OUTYM        0x07      /**< outym register */
#define HMC5883_REG_OUTYL        0x08      /**< outyl register */

#define HMC5883_REG_STATUS       0x09      /**< status register */
#define HMC5883_REG_IDA          0x0A      /**< ida register */
#define HMC5883_REG_IDB          0x0B      /**< idb register */
#define HMC5883_REG_IDC          0x0C      /**< idc register */

unsigned char HMC5883LInit(void);

unsigned char HMC5883L_Read_Byte(unsigned char addr);
void HMC5883LRead(HMC5883L_t * DataStruct);

float HMC5883LCalculateHeading(HMC5883L_t *data);

#ifdef __cplusplus
}
#endif
#endif