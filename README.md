# EDF-Schedular
Implementing EDF Scheduler based on FreeRTOS
to Enable the EDF you have to make the "#define configUSE_EDF_SCHEDULER"  1   in FreeRTOSConfig.h
and use xTaskCreatePeriodic when creating any task.
you can see the Verfication of EDF.pdf file to see the results when tested.
