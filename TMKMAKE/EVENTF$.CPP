#include "eventf"
#include "oli"

#include "MAPINC/cmddefs.h"
#include "MAPINC/evfevent.h"

#include <xxfdbk.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//***** PRIVATE VARIABLE DEFINITIONS ***********************************************************************************
namespace { namespace parms {

const char* EVFEVENT = "EVFEVENT";
bool        eventf_option;
std::string evfevent_libarary;

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


typedef std::pair<std::string, std::string> qname_t;
qname_t split_qname(const std::string& qname)
{
  auto s = qname.find_first_of("/");
  if (s == std::string::npos)
    return std::make_pair(std::string(), qname);

  return std::make_pair(qname.substr(0, s), qname.substr(s + 1));
}


oli::severity_t
evfevent_method(oli::command& mfCmd, const oli::file_name& evfeventName, char process)
{
  mfCmd.add_keyword_value("OPTION", "*EVENTF");
  auto severity = mfCmd.execute();
  if (process == oli::BLANK)
    return severity;

  return severity;
}


void
copy_evfevent(const oli::file_name& src, char type)
{
  // Set the name of the target member
  auto dst = src;
  dst.library = parms::evfevent_libarary;
  dst.member.resize(10, '_');
  *dst.member.rbegin() = type;

  // Сopying member
  oli::command cmd;
  cmd = oli::format("CPYF FROMFILE(%s) FROMMBR(%s) TOFILE(%s) TOMBR(%s) "
                    "MBROPT(*REPLACE) FMTOPT(*NOCHK) CRTFILE(*YES)",
                    src.libq_name().c_str(), src.member.c_str(),
                    dst.libq_name().c_str(), dst.member.c_str());
  cmd.execute();

  // Garbage removal
  if (src.library == parms::evfevent_libarary)
    cmd = oli::format("RMVM FILE(%s) MBR(%s)", src.libq_name().c_str(),
                      src.member.c_str());
  else
    cmd = oli::format("DLTF FILE(%s)", src.libq_name().c_str());

  cmd.execute();
}

} // namespace { ///////////////////////////////////////////////////////////////////////////////////////////////////////





//***** PUBLIC FUNCTIONS DEFINITIONS ***********************************************************************************

// INITIALIZING EVFEVENT FILE CREATION MODULE
Void eventf_initialize(bool eventf_option, const char* makefile)
{
  parms::eventf_option = eventf_option;
  if (!eventf_option) return;


  // Determining the name of the library to host EVFEVENT
  auto mf = _Ropen(makefile, "rr");
  if (!mf) {
    parms::eventf_option = false;
    return;
  }

  auto fb = _Ropnfbk(mf);
  parms::evfevent_libarary = oli::get_string(fb->library_name);
  _Rclose(mf);


  // Create EVFEVENT if doesn't exist
  oli::command cmd;
  auto qname = parms::evfevent_libarary + '/' + parms::EVFEVENT;
  cmd = oli::format("CHKOBJ OBJ(%s) OBJTYPE(*FILE)", qname.c_str());

  if (cmd.execute()) {
    cmd = oli::format("CRTPF FILE(%s) RCDLEN(500) MAXMBRS(*NOMAX)", qname.c_str());
    if (cmd.execute()) parms::eventf_option = false;
    return;
  }


  // This crutch is needed to force RDi to fetch all messages from EVFEVENT, not just new ones.
  auto tmp = "EVFEVENT00";
  auto copy = oli::format("RNMOBJ OBJ(%s/%%s) OBJTYPE(*FILE) NEWOBJ(%%s)",
                          parms::evfevent_libarary.c_str());

  cmd = oli::format(copy.c_str(), parms::EVFEVENT, tmp);
  cmd.execute();
  cmd = oli::format(copy.c_str(), tmp, parms::EVFEVENT);
  cmd.execute();
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
  if (!parms::eventf_option)
    return mfCmd.execute();


  // Get CMDDEFS file description and process errors
  oli::file<CMDDEFS_both_t, CMDDEFS_key_t>* cmddefs;
  oli::current_job* currentJob;
  try {
    cmddefs = get_cmddefs();
    currentJob = get_current_job();
  } catch (std::exception& e) {
    parms::eventf_option = false;
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
  auto qnameObj = split_qname(mfCmd.keyword_value(parmName));
  if (qnameObj.first.empty())
  {
    auto curlib = currentJob->current_library();
    if (curlib.empty())
      return EXEC_OBJLIB_EMPTY;
    qnameObj.first = curlib;
  }


  // Executing a command with appropriate error handling
  oli::file_name srcEventf(qnameObj.first, "EVFEVENT", qnameObj.second);
  oli::severity_t severity = 0;
  if (cmdDef->METHOD == 'E')
    severity = evfevent_method(mfCmd, srcEventf, cmdDef->PROCESS);
//  else
//    severity = joblog_method(mfCmd, srcEventf);


  //
  copy_evfevent(srcEventf, cmdDef->TYPE);
  return severity;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

