#include <appConfig.h>
#include <systemInit.h>
#include <pdsTask.h>
#include <validation.h>


void main(void)
{
#if ENABLE_FUNCTION_VALIDATION == 1
    validation_main();
#endif
    /* !!!!!!!MUST BE called firstly here for initializing system parameters !!!!*/
    PDS_Init();
    /* System init for hardwre init */
    SYS_Init();
    /* system main infinite loop */
    for(;;){
        TM_RunTasks();
    }
}
