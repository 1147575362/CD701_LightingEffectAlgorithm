#ifndef LED_TABLES_H
#define LED_TABLES_H

#include <colorFormula.h>
#include <pdsTask.h>

/*  Please don't do any change for it would be used in Color mixing task */
#define TEMP_CMP_NUM_MAX          (61U)

#define MINIMUM_TEMP              (0)
#define MAXIMUM_TEMP              (120)
#define DELTA_TEMP_POS            (1U)
#define DELTA_TEMP                (1U << DELTA_TEMP_POS)/*must be 2^DELTA_TEMP_POS*/

typedef struct{
    uint16_t intensityEfficiency; /*efficiency:100.0% means 1024*/
    int16_t deltaVoltPN;          /*mV*/
    uint16_t x;                   /*float rate: 100.0% means 1024*/
    uint16_t y;                   /*float rate: 100.0% means 1024*/
}TempParams_t;

TempParams_t const * LP_GetTempTableRed(LedNum_t ledIndex);
TempParams_t const * LP_GetTempTableGreen(LedNum_t ledIndex);
TempParams_t const * LP_GetTempTableBlue(LedNum_t ledIndex);


























#endif