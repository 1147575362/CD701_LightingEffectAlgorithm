/**************************************************
 *
 * This module contains the function `__low_level_init', a function
 * that is called before the `main' function of the program.  Normally
 * low-level initializations - such as setting the prefered interrupt
 * level or setting the watchdog - can be performed here.
 *
 * Note that this function is called before the data segments are
 * initialized, this means that this function cannot rely on the
 * values of global or static variables.
 *
 * When this function returns zero, the startup code will inhibit the
 * initialization of the data segments. The result is faster startup,
 * the drawback is that neither global nor static data will be
 * initialized.
 *
 * Copyright 1999-2004 IAR Systems. All rights reserved.
 *
 * $Revision: 2474 $
 *
 **************************************************/

#include <stdint.h>
#include <realplumLite.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma language=extended

__interwork int __low_level_init(void);

__interwork int __low_level_init(void)
{
    /*==================================*/
    /*  Initialize hardware.            */
    /*==================================*/
    
    /*
    Indie's Flash Initialization
    For the Cortex M0 core (CM0160K8)
    0 Read/Write Wait-State Clock Cycles [2:0]
    0 Write/Erase Clock Divider [5:3]
    */
    *( ( volatile uint32_t * ) 0x50020040 ) = 0x00000000U;

    /* Initial system clock to RC to allow faster boot up */
    CRGA_SFRS->SYSCLKCTRL.HFRCENA = 1U;
    while(CRGA_SFRS->SYSCLKCTRL.HFRCSTS == 0U){}
    CRGA_SFRS->SYSCLKCTRL.SYSCLKSEL = 1U;

    uint16_t rev = SYSCTRLA_SFRS->REV;
    if (SYSCTRLA_SFRS->NAME != 0x50454143U || (rev >> 8) != 0x42U){ /* 0x41='A', 0x42='B'  */
        /* invalid chip id*/
        for(;;){}
    }
    
    EVTHOLD_SFRS->HOLD = 0U;
    SYSCTRLA_REG_RETAIN0 = 0x01U; /*  figure out it's application mode   */
    PMUA_REG_CTRL.UPDATE = 1U;
    while(PMUA_REG_CTRL.UPDATE == 1U){}
    /* Stop watchdog on ASIC */
    WDTA_SFRS->STOP.STOP = 0xC3U;
    /*==================================*/
    /* Choose if segment initialization */
    /* should be done or not.           */
    /* Return: 0 to omit seg_init       */
    /*         1 to run seg_init        */
    /*==================================*/
    
    return 1;
}

#pragma language=default

#ifdef __cplusplus
}
#endif
