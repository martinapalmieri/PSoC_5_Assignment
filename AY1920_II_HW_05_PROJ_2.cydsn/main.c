/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

// Include header files
#include "I2C_Interface.h"
#include "project.h"
#include "stdio.h"

//Brief slave device address
#define LIS3DH_DEVICE_ADDRESS 0x18

//Brief WHO AM I register address
#define LIS3DH_WHO_AM_I_REG_ADDR 0x0F

//Brief STATUS REGISTER address
#define LIS3DH_STATUS_REG 0x27

//Brieg CONTROL REGISTER 1 address
#define LIS3DH_CTRL_REG1 0x20

/*Brief HEX value for CONTROL REGISTER 1: Normal mode at 100 Hz
CTRL_REG1[3]=LPen=0 (Normal mode);
CTRL_REG1[7:4]=ODR[3:0]=0101 (100 Hz)*/
#define LIS3DH_NORMAL_MODE_100_HZ_CTRL_REG1 0x57


//Brief CONTROL REGISTER 4 address
#define LIS3DH_CTRL_REG4 0x23
/*Brief HEX value for CONTROL REGISTER 4: +- 2.0 g FSR
CTRL_REG4[3]=0 (High resolution disabled)
CTRL_REG4[5:4]=FS[1:0]=00 (2.0 g FSR) */
#define LIS3DH_CTRL_REG4_2g 0x80

//Brief OUT_X_L register address (x-axis output LSB)
#define LIS3DH_OUT_X_L 0x28

//Brief OUT_Y_L register address (y-axis output LSB)
#define LIS3DH_OUT_Y_L 0x2A

//Brief OUT_Z_L register address (z-axis output LSB)
#define LIS3DH_OUT_Z_L 0x2C


int main(void)
{
    CyGlobalIntEnable; 
    
    //Initialization
    I2C_Peripheral_Start();
    UART_Debug_Start();
    
    //"The boot procedure is complete about 5 milliseconds after device power-up."
    CyDelay(5); 
    
    char message[50];
    //Reading CONTROL REGISTER 1
    uint8_t ctrl_reg1; 
    ErrorCode error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG1,
                                        &ctrl_reg1);
    
    //Writing CONTROL REGISTER 1
    if (ctrl_reg1 != LIS3DH_NORMAL_MODE_100_HZ_CTRL_REG1)
    {
        ctrl_reg1 = LIS3DH_NORMAL_MODE_100_HZ_CTRL_REG1;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                             LIS3DH_CTRL_REG1,
                                             ctrl_reg1);
    }
   
    // Reading CONTROL REGISTER 4   
    uint8_t ctrl_reg4;
    error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_CTRL_REG4,
                                        &ctrl_reg4);
    // Writing CONTROL REGISTER 4
    if (ctrl_reg4 != LIS3DH_CTRL_REG4_2g)
    {
        ctrl_reg4 = LIS3DH_CTRL_REG4_2g;
        error = I2C_Peripheral_WriteRegister(LIS3DH_DEVICE_ADDRESS,
                                         LIS3DH_CTRL_REG4,
                                         ctrl_reg4);
    }
    
    
    
    for(;;)
    {
        
    }
}

/* [] END OF FILE */
