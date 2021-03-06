/* ========================================
 * \file main.c
 *
 * Source code for reading output data from 
 * a LIS3DH tri-axial accelerometer in High Resolution
 * Mode at 100 Hz. 
 * 
 * Output data is converted in mg units, while
 * the conversion in m/s^2 units is perfomed
 * in the Bridge Control Panel Variable Setting
 * feature ( see HW_05_PALMIERI_MARTINA.ini for
 * details).
 *
 * ========================================
*/

// Include header files
#include "I2C_Interface.h"
#include "InterruptRoutines.h"
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

/*Brief HEX value for CONTROL REGISTER 1: High resolution mode at 100 Hz
CTRL_REG1[3]=LPen=0 (HR mode);
CTRL_REG1[7:4]=ODR[3:0]=0101 (100 Hz)*/
#define LIS3DH_NORMAL_MODE_100_HZ_CTRL_REG1 0x57


//Brief CONTROL REGISTER 4 address
#define LIS3DH_CTRL_REG4 0x23
/*Brief HEX value for CONTROL REGISTER 4: +- 4.0 g FSR
CTRL_REG4[3]=1 (High resolution enabled)
CTRL_REG4[5:4]=FS[1:0]=01 (4.0 g FSR) */
#define LIS3DH_CTRL_REG4_2g 0x98

//Brief OUT_X_L register address (x-axis output LSB)
#define LIS3DH_OUT_X_L 0x28

//Brief OUT_Y_L register address (y-axis output LSB)
#define LIS3DH_OUT_Y_L 0x2A

//Brief OUT_Z_L register address (z-axis output LSB)
#define LIS3DH_OUT_Z_L 0x2C

//Brief HEADER and FOOTER values for UART communication
#define HEADER 0xA0
#define FOOTER 0xC0

//Brief value of sensitivity in High Resolution mode (2 mg/digit)
#define HR_SENSITIVITY 2;

extern uint8_t flag;

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
    
     //Brief output acceleration data variables:
    uint8_t AccelerationData[6];
   
    //x-axis
    int16_t X_Out;
    int16_t X_Out_mg;
    //y-axis
    int16_t Y_Out;
    int16_t Y_Out_mg;
    //z-axis
    int16_t Z_Out;
    int16_t Z_Out_mg;
    
    uint8_t OutArray[8];
    
    
    
    //Header and footer for communication w/ Bridge Control Panel
    OutArray[0]=HEADER;
    OutArray[7]=FOOTER;
    
    for(;;)
    {
        if (flag == 1){
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
                        X_Out=(int16)(AccelerationData[0] | (AccelerationData[1] << 8)) >> 4;
                        //Data * sensitivity (HR mode) = [mg] (x-axis)
                        X_Out_mg= X_Out*HR_SENSITIVITY;
                        //MSB (x-axis)
                        OutArray[1]=(uint8_t)(X_Out_mg >> 8);
                        //LSB (x-axis)
                        OutArray[2]=(uint8_t)(X_Out_mg & 0xFF);
                        
                        // Conversion of output data into right-justified 16 bit int (y-axis)
                        Y_Out=(int16)(AccelerationData[2] | (AccelerationData[3] << 8)) >> 4;
                        //Data * sensitivity (HR mode) = [mg] (x-axis)
                        Y_Out_mg=Y_Out*HR_SENSITIVITY;
                        //MSB (x-axis)
                        OutArray[3]=(uint8_t)(Y_Out_mg >> 8);
                        //LSB (x-axis)
                        OutArray[4]=(uint8_t)(Y_Out_mg & 0xFF);
                        
                        // Conversion of output data into right-justified 16 bit int (z-axis)
                        Z_Out=(int16)(AccelerationData[4] | (AccelerationData[5] << 8)) >> 4;
                        //Data * sensitivity (HR mode) = [mg] (x-axis)
                        Z_Out_mg=Z_Out*HR_SENSITIVITY;
                        //MSB (x-axis)
                        OutArray[5]=(uint8_t)(Z_Out_mg >> 8);
                        //LSB (x-axis)
                        OutArray[6]=(uint8_t)(Z_Out_mg & 0xFF);
                    
                        UART_Debug_PutArray(OutArray,8);
                    }
                }   
            } 
        }
    }
}

/* [] END OF FILE */
