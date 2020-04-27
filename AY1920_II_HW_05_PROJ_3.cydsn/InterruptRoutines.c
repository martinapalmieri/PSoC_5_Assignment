/* ========================================
 *
 * \file InterruptRoutines.c 

 * Source code for interrupt implementation
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
