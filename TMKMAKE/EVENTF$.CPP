#include "eventf"
#include "oli"

#include "MAPINC/cmddefs.h"
#include "MAPINC/evfevent.h"

#include <xxfdbk.h>
#include <ctime>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//***** PRIVATE TYPES **************************************************************************************************
namespace { namespace type {

typedef std::pair<std::string, std::string> qname_t;


typedef decltype(*oli::current_job::joblog().begin()) joblog_message;


class evfevent
{
  typedef std::string string;
  oli::file_name m_name;
  oli::file<EVFEVENT_both_t> m_file;


  string inspect(bool clear) {
    const char* chkfile = "CHKOBJ OBJ(%s) OBJTYPE(*FILE) MBR(%s)";
    oli::command cmd;

    // If the EVFEVENT file is missing from the 'lib'
    cmd = oli::format(chkfile, m_name.libq_name().c_str(), "*NONE");
    if (cmd.execute()) {
      cmd = oli::format("CRTPF FILE(%s) MBR(%s) RCDLEN(500) MAXMBRS(*NOMAX)",
                        m_name.libq_name().c_str(), m_name.member.c_str());
      cmd.execute();
      return m_name.full_name();
    }

    // If the EVFEVENT member is missing from the file EVFEVENT
    cmd = oli::format(chkfile, m_name.libq_name().c_str(), m_name.member.c_str());
    if (cmd.execute()) {
      cmd = oli::format("ADDPFM FILE(%s) MBR(%s)",
                        m_name.libq_name().c_str(), m_name.member.c_str());
      cmd.execute();
      return m_name.full_name();
    }

    // If need to clear target member
    if (clear) {
      cmd = oli::format("CLRPFM FILE(%s) MBR(%s)",
                        m_name.libq_name().c_str(), m_name.member.c_str());
      cmd.execute();
    }

    return m_name.full_name();
  }


  public:
  static const char* NAME;


  evfevent(bool clear, const string& lib, const string& mbr = NAME)
  : m_name(lib, NAME, mbr), m_file(inspect(clear), "rr+") {}


  void add_prolog() {
    EVFEVENT_both_t rec;
    auto rawdt = std::time(nullptr);
    auto ltime = std::localtime(&rawdt);
    strftime(rec.EVFEVENT, sizeof(rec.EVFEVENT), "%Y%m%d%H%M%S", ltime);
    std::string ts(rec.EVFEVENT);

    oli::fill_string(rec.EVFEVENT, oli::format("TIMESTAMP  0 %s", ts.c_str()));
    m_file.write(rec);

    oli::fill_string(rec.EVFEVENT, "PROCESSOR  1 000 1");
    m_file.write(rec);
  }


  void add_message(const joblog_message& msg) {
    auto type = oli::get_string(msg.detail.Message_Type);

    char sign = 'I';
    if (type == "15") sign = 'E';
    else if(type == "02")
      sign = msg.detail.Message_Severity <= 10 ? 'W' : 'S';

    auto text = oli::format("ERROR      1 000 2 000000 000000 001 000000 001 %.7s %c %02d %03d %s",
                            msg.detail.Message_Id, sign, msg.detail.Message_Severity,
                            msg.text.length(), msg.text.c_str());

    EVFEVENT_both_t rec;
    oli::fill_string(rec.EVFEVENT, text);
    m_file.write(rec);
  }
};

const char* evfevent::NAME = "EVFEVENT";

}} // namespace { namespace type { /////////////////////////////////////////////////////////////////////////////////////





//***** PRIVATE VARIABLE DEFINITIONS ***********************************************************************************
namespace { namespace parms {

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



type::qname_t
split_qname(const std::string& qname)
{
  auto s = qname.find_first_of("/");
  if (s == std::string::npos)
    return std::make_pair(std::string(), qname);

  return std::make_pair(qname.substr(0, s), qname.substr(s + 1));
}



oli::severity_t
evfevent_method(oli::command& cmd, const oli::file_name& efname)
{
  // efname нужен для дальнейшей доработки - очистке получившегося EVFEVENT
  cmd.add_keyword_value("OPTION", "*EVENTF");
  return cmd.execute();
}



oli::severity_t
joblog_method(oli::command& cmd, const oli::file_name& efname)
{
  auto joblog   = oli::current_job::joblog();
  auto it       = joblog.last();
  auto severity = cmd.execute();
  auto end      = joblog.last();

  type::evfevent evfevent(true, efname.library, efname.member);
  evfevent.add_prolog();

  while (++it != end)
    evfevent.add_message(*it);

  return severity;
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
  try {
    type::evfevent evfevent(true, parms::evfevent_libarary);
    evfevent.add_prolog();
  } catch (std::exception& e) {
    parms::eventf_option = false;
    return;
  }


  // This crutch is needed to force RDi to fetch all messages from EVFEVENT, not just new ones
  auto tmp = std::string(type::evfevent::NAME) + "00";
  auto copy = oli::format("RNMOBJ OBJ(%s/%%s) OBJTYPE(*FILE) NEWOBJ(%%s)",
                          parms::evfevent_libarary.c_str());

  oli::command cmd;
  cmd = oli::format(copy.c_str(), type::evfevent::NAME, tmp.c_str());
  cmd.execute();
  cmd = oli::format(copy.c_str(), tmp.c_str(), type::evfevent::NAME);
  cmd.execute();
}



// EXECUTE MAKEFILE COMMAND
Int16 eventf_execute_command(const char* mfCmd)
{
  // If the command has invalid syntax
  static oli::command cmd;
  try {
    cmd = mfCmd;
  } catch (std::exception& e) {
    return EXEC_INVALID_CMD;
  }


  // If *EVENTF option not set for TMKMAKE
  if (!parms::eventf_option)
    return cmd.execute();


  // Get CMDDEFS file description and process errors
  oli::file<CMDDEFS_both_t, CMDDEFS_key_t>* cmddefs;
  oli::current_job* currentJob;
  try {
    cmddefs = get_cmddefs();
    currentJob = get_current_job();
  } catch (std::exception& e) {
    parms::eventf_option = false;
    return cmd.execute();
  }


  // If command name not in CMDDEFS file
  CMDDEFS_key_t key;
  oli::fill_string(key.NAME, cmd.name());
  auto cmdDef = cmddefs->find(key);
  if (cmdDef == cmddefs->end())
    return cmd.execute();


  // Get info of the created object
  auto parmName = oli::get_string(cmdDef->OBJECT);
  auto qnameObj = split_qname(cmd.keyword_value(parmName));
  if (qnameObj.first.empty()) {
    auto curlib = currentJob->current_library();
    if (curlib.empty())
      return EXEC_OBJLIB_EMPTY;
    qnameObj.first = curlib;
  }


  // Executing a command with appropriate error handling
  oli::file_name src(qnameObj.first, type::evfevent::NAME, qnameObj.second);
  oli::severity_t severity = 0;
  if (cmdDef->METHOD == 'E')
    severity = evfevent_method(cmd, src);
  else
    severity = joblog_method(cmd, src);


  // Set the name of the target member EVFEVENT
  auto dst = src;
  dst.library = parms::evfevent_libarary;
  dst.member.resize(10, '_');
  *dst.member.rbegin() = cmdDef->TYPE;


  // Сopying member source EVFEVENT to destination
  cmd = oli::format("CPYF FROMFILE(%s) FROMMBR(%s) TOFILE(%s) TOMBR(%s) "
                    "MBROPT(*REPLACE) FMTOPT(*NOCHK) CRTFILE(*YES)",
                    src.libq_name().c_str(), src.member.c_str(),
                    dst.libq_name().c_str(), dst.member.c_str());
  cmd.execute();


  // Garbage removal
  cmd = src.library == parms::evfevent_libarary ?
        oli::format("RMVM FILE(%s) MBR(%s)", src.libq_name().c_str(), src.member.c_str()) :
        oli::format("DLTF FILE(%s)", src.libq_name().c_str());
  cmd.execute();


  return severity;
}



// ADD LAST JOBLOG MESSAGE TO EVFEVENT
Void eventf_add_message(void)
{
  if (!parms::eventf_option)
    return;

  static type::evfevent evfevent(false, parms::evfevent_libarary);
  evfevent.add_message(*oli::current_job::joblog().last());
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

