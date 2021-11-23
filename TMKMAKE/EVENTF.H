#ifndef TMKMAKE_EVENTF_H
#define TMKMAKE_EVENTF_H

#include "base"
#include <xxfdbk.h>
#include <stdbool.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//***** PUBLIC FUNCTION PROTOTYPES *************************************************************************************
#ifdef __cplusplus
extern "C" {
#endif

Void  eventf_set_flag_eventf(bool flag);                        // using in option$.c
Void  eventf_set_dstlib(const _XXOPFB_T* fb);                   // using in parser$.c
Int16 eventf_execute_command(const char* cmd);                  // using in mfproc$.c

#ifdef __cplusplus
} // extern "C" {
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#endif

