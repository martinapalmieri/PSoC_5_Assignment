/* ========================================
 * \file main.c
 *
 * Source code for reading output data from 
 * a LIS3DH tri-axial accelerometer in Normal Mode
 * at 100 Hz. 
 * 
   
 *
 * ========================================
*/

// Include header files
#include "InterruptRoutines.h"
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

int main(void)
{
    CyGlobalIntEnable; 
    
    //Initialization
    Timer_LISD3H_Start();
    I2C_Peripheral_Start();
    UART_Debug_Start();
    ISR_DataReady_StartEx(DataReady_ISR);
    
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
    
    //Brief output acceleration data variables
    uint8_t AccelerationData[6];
    
    int16_t X_Out;
    
    int16_t Y_Out;
    
    int16_t Z_Out;
    
    uint8_t OutArray[8];
    
    //Header and footer for communication w/ Bridge Control Panel
    OutArray[0]=HEADER;
    OutArray[7]=FOOTER;
    
    extern uint8_t flag; 
    
    for(;;)
    {
       if (flag == 1)
        { 
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
                        //MSB (x-axis)
                        OutArray[1]=(uint8_t)(X_Out >> 8); 
                        //LSB (x-axis)
                        OutArray[2]=(uint8_t)(X_Out & 0xFF);
                    
                        // Conversion of output data into right-justified 16 bit int (y-axis)
                        Y_Out=(int16)(AccelerationData[2] | (AccelerationData[3]<<8)) >> 6;
                        //MSB (y-axis)
                        OutArray[3]=(uint8_t)(Y_Out >> 8);
                         //LSB (y-axis)
                        OutArray[4]=(uint8_t)(Y_Out & 0xFF);
                    
                        // Conversion of output data into right-justified 16 bit int (z-axis)
                        Z_Out=(int16)(AccelerationData[4] | (AccelerationData[5]<<8)) >> 6;
                        //MSB (z-axis)
                        OutArray[5]=(uint8_t)(Z_Out >> 8);
                        //LSB (z-axis)
                        OutArray[6]=(uint8_t)(Z_Out & 0xFF);
                
                        UART_Debug_PutArray(OutArray,8);
                    }
                }
            }
        } 
    }
}

/* [] END OF FILE */
