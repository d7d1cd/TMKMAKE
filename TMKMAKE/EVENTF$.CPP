#include "eventf"
#include "oli"

#include "MAPINC/cmddefs.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//***** PRIVATE VARIABLE DEFINITIONS ***********************************************************************************
namespace { namespace parms {

bool flag_eventf;
std::string dstlib;

}} // namespace { namespace parms { ////////////////////////////////////////////////////////////////////////////////////





//***** PRIVATE FUNCTIONS DEFINITIONS **********************************************************************************
namespace {

oli::file<CMDDEFS_both_t, CMDDEFS_key_t>* get_cmddefs()
{
  static oli::file<CMDDEFS_both_t, CMDDEFS_key_t> cmddefs("*LIBL/CMDDEFS");
  return &cmddefs;
}

} // namespace { ///////////////////////////////////////////////////////////////////////////////////////////////////////





//***** PUBLIC FUNCTIONS DEFINITIONS ***********************************************************************************

// SETTING THE FLAG OF THE NEED TO CREATE AN EVFEVENT FILE
Void eventf_set_flag_eventf(bool flag)
{
  parms::flag_eventf = flag;
}



// DETERMINING THE NAME OF THE LIBRARY TO HOST THE EVFEVENT FILE
Void eventf_set_dstlib(const _XXOPFB_T* fb)
{
  parms::dstlib = oli::get_string(fb->library_name);
}



// EXECUTE MAKEFILE COMMAND
Int16 eventf_execute_command(const char* cmd)
{
  // If the command has invalid syntax
  oli::command mfCmd(cmd);
  if (!mfCmd.check_syntax())
    return -1;


  // If *EVENTF option not set for TMKMAKE
  if (!parms::flag_eventf)
    return mfCmd.execute();


  // Get CMDDEFS file description and process errors
  oli::file<CMDDEFS_both_t, CMDDEFS_key_t>* cmddefs;
  try {
    cmddefs = get_cmddefs();
  } catch (std::exception& e) {
    parms::flag_eventf = false;
    return mfCmd.execute();
  }


  // If command name not in CMDDEFS file
  CMDDEFS_key_t key;
  oli::fill_string(key.NAME, mfCmd.name());
  auto cmdDef = cmddefs->find(key);
  if (cmdDef == cmddefs->end())
    return mfCmd.execute();


  throw 1;
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

