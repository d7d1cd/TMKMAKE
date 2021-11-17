#include "eventf"
#include <string>
#include <qcmdexc.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//***** VARIABLES DEFINITIONS ******************************************************************************************
namespace {

bool        g_flag_eventf;                  //
std::string g_dstlib;                       // Location for evfevent file

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





//***** SETTING THE FLAG OF THE NEED TO CREATE AN EVFEVENT FILE ********************************************************
Void eventf_set_flag_eventf(bool flag)
{
  g_flag_eventf = flag;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//***** DETERMINING THE NAME OF THE LIBRARY TO HOST THE EVFEVENT FILE **************************************************
Void eventf_set_dstlib(const _XXOPFB_T* fb)
{
  g_dstlib = std::string(fb->library_name, sizeof(fb->library_name));
  g_dstlib.erase(g_dstlib.find_first_of('\x40'));
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//**********************************************************************************************************************
Int16 eventf_execute_command(const char* command)
{
  if (g_flag_eventf)
    printf("Create EVENTF '%s'\n", command);
  else
    printf("NO create EVENTF '%s'\n", command);

  return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//// =================================================================
////  Function:    system_cmd_trap ()
//// =================================================================
//Static short system_cmd_sev;
//
//Static
//    Void
//    system_cmd_trap(int sig)
//{
//#ifdef __ILEC400__
//  _INTRPT_Hndlr_Parms_T excinfo; // exception data structure
//#else
//  sigdata_t *data; // pointer to exception data area
//  sigact_t *act;   // pointer to exception action area
//#endif
//
//  // Set ptr to sigdata structure
//#ifdef __ILEC400__
//  _GetExcData(&excinfo);
//#else
//  data = sigdata();
//#endif
//
//#ifdef __ILEC400__
//  system_cmd_sev = excinfo.Severity;
//#else
//  system_cmd_sev = data->exmsg->exmsgsev;
//#endif
//}
//
////**********************************************************************
//// Note:   system specific routines
////**********************************************************************
//// =================================================================
////  Function:    convert_to_pack ()
//// =================================================================
//Static
//    Void
//    convert_to_pack(Char *buf, Int16 rm, Int16 num)
//{
//  Char pc;
//
//  while (num--)
//  {
//    pc = rm % 10;
//    rm /= 10;
//    pc |= (rm % 10) * 16;
//    rm /= 10;
//    buf[num] = pc;
//  }
//}
