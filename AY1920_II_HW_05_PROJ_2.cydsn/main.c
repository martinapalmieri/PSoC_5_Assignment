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

//Brief HEADER and FOOTER values for UART communication
#define HEADER 0xA0
#define FOOTER 0xC0

//Brief value of sensitivity in Normal Mode (4 mg/digit)
#define NORMAL_MODE_SENSITIVITY 4;

int main(void)
{
    CyGlobalIntEnable; 
    
    //Initialization
    I2C_Peripheral_Start();
    UART_Debug_Start();
    
    //"The boot procedure is complete about 5 milliseconds after device power-up."
    CyDelay(5); 
   
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
    
    //Brief output acceleration data variables:
    uint8_t AccelerationData[6];
    
    int16_t X_Out;
    int16_t X_Out_mg;
    
    int16_t Y_Out;
    int16_t Y_Out_mg;
    
    int16_t Z_Out;
    int16_t Z_Out_mg;
    
    uint8_t OutArray[8];
   
    
    //Header and footer for communication w/ Bridge Control Panel
    OutArray[0]=HEADER;
    OutArray[7]=FOOTER;
    
    for(;;)
    {
        CyDelay(100);
        
       //Reading STATUS REGISTER to check for data availability
       uint8_t status_reg;
       error = I2C_Peripheral_ReadRegister(LIS3DH_DEVICE_ADDRESS,
                                        LIS3DH_STATUS_REG,
                                        &status_reg);
        if (error == NO_ERROR)
        {
            // Check if new data is available (STATUS_REG[3]=ZYXDA=1)
            if ((status_reg & 0x08) > 0)
            {
                    //Multiple register reading starting from OUT_X_L
                    error=I2C_Peripheral_ReadRegisterMulti(LIS3DH_DEVICE_ADDRESS,
												LIS3DH_OUT_X_L, 6,
												AccelerationData); 
                    if (error == NO_ERROR)
                    {
                    
                        // Conversion of output data into right-justified 16 bit int (x-axis)
                        X_Out=(int16)(AccelerationData[0] | (AccelerationData[1] << 8)) >> 6;
                        // Data * sensitivity -> output data in mg (x-axis)
                        X_Out_mg=X_Out*NORMAL_MODE_SENSITIVITY; 
                        //LSB (x-axis)
                        OutArray[1]=(uint8_t)(X_Out_mg & 0xFF);
                        //MSB (x-axis)
                        OutArray[2]=(uint8_t)(X_Out_mg >> 8);
                        
                        // Conversion of output data into right-justified 16 bit int (y-axis)
                        Y_Out=(int16)(AccelerationData[2]|(AccelerationData[3]<<8))>>6;
                        // Data * sensitivity -> output data in mg (y-axis)
                        Y_Out_mg=Y_Out*NORMAL_MODE_SENSITIVITY; 
                        //LSB (y-axis)
                        OutArray[3]=(uint8_t)(Y_Out_mg & 0xFF);
                        //MSB (y-axis)
                        OutArray[4]=(uint8_t)(Y_Out_mg >> 8);
                        
                        // Conversion of output data into right-justified 16 bit int (z-axis)
                        Z_Out=(int16)(AccelerationData[4]|(AccelerationData[5]<<8))>>6;
                        // Data * sensitivity -> output data in mg (z-axis)
                        Z_Out_mg=Z_Out*NORMAL_MODE_SENSITIVITY; 
                        //LSB (z-axis)
                        OutArray[5]=(uint8_t)(Z_Out_mg & 0xFF);
                        //MSB (z-axis)
                        OutArray[6]=(uint8_t)(Z_Out_mg >> 8);
                    
                         UART_Debug_PutArray(OutArray,8);
                    }
            }
        } 
        
    }
}

/* [] END OF FILE */
