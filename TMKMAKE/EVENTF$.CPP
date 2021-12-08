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

oli::file<CMDDEFS_both_t, CMDDEFS_key_t>*
get_cmddefs()
{
  static oli::file<CMDDEFS_both_t, CMDDEFS_key_t> cmddefs("*LIBL/CMDDEFS");
  return &cmddefs;
}


oli::current_job*
get_current_job()
{
  static oli::current_job currentJob;
  return &currentJob;
}


std::pair<std::string, std::string>
split_qname(const std::string& qname)
{
  auto s = qname.find_first_of("/");
  if (s == std::string::npos)
    return std::make_pair(std::string(), qname);

  return std::make_pair(qname.substr(0, s), qname.substr(s + 1));
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
  static oli::command mfCmd;
  try {
    mfCmd = cmd;
  } catch (std::exception& e) {
    return EXEC_INVALID_CMD;
  }


  // If *EVENTF option not set for TMKMAKE
  if (!parms::flag_eventf)
    return mfCmd.execute();


  // Get CMDDEFS file description and process errors
  oli::file<CMDDEFS_both_t, CMDDEFS_key_t>* cmddefs;
  oli::current_job* currentJob;
  try {
    cmddefs = get_cmddefs();
    currentJob = get_current_job();
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


  // Get info of the created object
  auto parmName = oli::get_string(cmdDef->OBJECT);
  auto qname = split_qname(mfCmd.keyword_value(parmName));
  auto cmdObject  = qname.second;
  auto cmdLibrary = qname.first.empty() ? currentJob->current_library() :
                                          qname.first;
  if (cmdLibrary.empty())
    return EXEC_OBJLIB_EMPTY;



  printf("cmdLibrary = '%s', cmdObject = '%s'\n", cmdLibrary.c_str(), cmdObject.c_str());
//  throw 1;
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

