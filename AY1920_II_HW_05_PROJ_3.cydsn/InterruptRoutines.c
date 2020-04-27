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
#include "InterruptRoutines.h"
#include "Timer_LISD3H.h"

uint8_t flag = 0;

CY_ISR(DataReady_ISR)
{
    //Put interrupt line low
    Timer_LISD3H_ReadStatusRegister();
    
    flag=1;
    
}

/* [] END OF FILE */
