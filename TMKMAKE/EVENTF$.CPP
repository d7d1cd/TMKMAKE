#include "eventf"
#include <string>
#include <algorithm>
#include <qcmdexc.h>
#include <qcmdchk.h>
#include <signal.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//***** TYPES DECLARATIONS *********************************************************************************************
typedef decltype(_INTRPT_Hndlr_Parms_T::Severity) severity_t;
typedef decltype(QCMDEXC) command_processor;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





//***** CLASSES DECLARATIONS *******************************************************************************************
namespace {

class Command
{
  public:
  Command(const char* cmd);
  bool check_syntax() const;
  severity_t execute() const;
  std::string name() const;

  private:
  bool process_command(command_processor cmd_proc) const;

  private:
  std::string m_command;
};

} // namespace { ///////////////////////////////////////////////////////////////////////////////////////////////////////





//***** COMMAND CLASS DEFINITION ***************************************************************************************
namespace {

// PROCESSING OF SYSTEM SIGNALS
_INTRPT_Hndlr_Parms_T g_exception_info;
void system_command_trap(int sig)
{
  _GetExcData(&g_exception_info);
}


// CONSTRUCT COMMAND
Command::Command(const char* cmd)
       : m_command(cmd)
{
  auto begin = m_command.begin();
  std::transform(begin, m_command.end(), begin, toupper);
}


// EXECUTE OR CHECK COMMAND
bool Command::process_command(command_processor cmd_proc) const
{
  g_exception_info.Severity = 0;
  auto cmd = m_command;

  auto old_handler = signal(SIGALL, system_command_trap);
  cmd_proc(&cmd[0], cmd.length());
  signal(SIGALL, old_handler);

  return g_exception_info.Severity == 0;
}


// CHECK COMMAND SYNTAX
bool Command::check_syntax() const
{
  return process_command(QCMDCHK);
}


// EXECUTE COMMAND
severity_t Command::execute() const
{
  process_command(QCMDEXC);
  return g_exception_info.Severity;
}


// RETRIEVE COMMAND NAME
std::string Command::name() const
{
  auto end = m_command.find_first_of('\x40');
  return m_command.substr(0, end);
}

} // namespace { ///////////////////////////////////////////////////////////////////////////////////////////////////////





//***** PRIVATE VARIABLE DEFINITIONS ***********************************************************************************
namespace {

bool        g_flag_eventf;
std::string g_dstlib;

} // namespace { ///////////////////////////////////////////////////////////////////////////////////////////////////////





//***** PUBLIC FUNCTIONS DEFINITIONS ***********************************************************************************

// SETTING THE FLAG OF THE NEED TO CREATE AN EVFEVENT FILE
Void eventf_set_flag_eventf(bool flag)
{
  g_flag_eventf = flag;
}



// DETERMINING THE NAME OF THE LIBRARY TO HOST THE EVFEVENT FILE
Void eventf_set_dstlib(const _XXOPFB_T* fb)
{
  g_dstlib = std::string(fb->library_name, sizeof(fb->library_name));
  g_dstlib.erase(g_dstlib.find_first_of('\x40'));
}



// EXECUTE MAKEFILE COMMAND
Int16 eventf_execute_command(const char* cmd)
{
  // Check command syntax
  Command mfCmd(cmd);
  if (!mfCmd.check_syntax())
    return -1;

  auto cmdName = mfCmd.name();
  printf("'%s'\n", cmdName.c_str());

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

