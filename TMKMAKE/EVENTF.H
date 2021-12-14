#ifndef TMKMAKE_EVENTF_H
#define TMKMAKE_EVENTF_H

#include "base"
#include <stdbool.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//***** PUBLIC FUNCTION PROTOTYPES *************************************************************************************
#ifdef __cplusplus
extern "C" {
#endif

Void  eventf_initialize(bool, const char*);                   // using in option$.c
Int16 eventf_execute_command(const char*);                    // using in mfproc$.c

#ifdef __cplusplus
} // extern "C" {
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#endif

