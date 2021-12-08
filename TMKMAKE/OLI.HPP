#ifndef TMKMAKE_OLI_H
#define TMKMAKE_OLI_H

#define __IBMCPP_TR1__ 1
#include <type_traits>
#undef __IBMCPP_TR1__

#include <signal.h>
#include <string>
#include <qcmdexc.h>
#include <qcmdchk.h>
#include <algorithm>
#include <recio.h>
#include <qusrjobi.h>
#include <new>
#include <vector>
#include <qusec.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//***** STANDARD LIBRARY IMPLEMENTATION ********************************************************************************
namespace stl {

// enable_if
template<bool B, typename T = void>
struct enable_if {};

template<typename T>
struct enable_if<true, T> { typedef T type; };


// remove_reference
template <typename T>
struct remove_reference      { typedef T type; };

template <typename T>
struct remove_reference<T&>  { typedef T type; };

template <typename T>
struct remove_reference<T&&> { typedef T type; };


// move
template <typename T>
typename remove_reference<T>::type&& move(T&& t) {
  return static_cast<typename remove_reference<T>::type&&>(t);
}

} // namespace stl /////////////////////////////////////////////////////////////////////////////////////////////////////





//***** GENERAL PURPOSE FUNCTIONS **************************************************************************************
namespace oli {

const char BLANK = '\x40';


template <size_t N>
std::string get_string(const char (&src)[N])
{
  auto text = std::string(src, N);
  text.erase(text.find_last_not_of(BLANK) + 1);
  return text;
}


template <size_t N>
void fill_string(char (&dst)[N], const std::string& src)
{
  auto less = std::min(N, src.length());
  std::copy(src.begin(), src.begin() + less, dst);
  std::fill(dst + less, dst + N, BLANK);
}

} // namespace oli { ///////////////////////////////////////////////////////////////////////////////////////////////////





//***** ISTRING CLASS DECLARATION **************************************************************************************
namespace oli {

template <size_t N>
class istring
{
  char m_data[N];


  public:
  istring() {
    clear();
  }


  istring(const char* src) {
    clear();
    auto less = std::min(N, std::strlen(src));
    std::copy(src, src + less, m_data);
  }

  void clear() {
    std::fill(m_data, m_data + N, BLANK);
  }
};

} // namespace oli { ///////////////////////////////////////////////////////////////////////////////////////////////////





//***** COMMAND CLASS DECLARATION **************************************************************************************
namespace oli {

// Types declarations
typedef decltype(_INTRPT_Hndlr_Parms_T::Severity) severity_t;
typedef decltype(QCMDEXC) command_processor;


// Processing of system signals
_INTRPT_Hndlr_Parms_T g_exception_info;
void system_command_trap(int sig) {
  _GetExcData(&g_exception_info);
}


// "command" class declaration
class command
{
  static const std::string EVENTF_OPTION;
  std::string m_command;


  bool process_command(command_processor cmd_proc) const {
    g_exception_info.Severity = 0;
    auto cmd = m_command;

    auto old_handler = signal(SIGALL, system_command_trap);
    cmd_proc(&cmd[0], cmd.length());
    signal(SIGALL, old_handler);

    return g_exception_info.Severity == 0;
  }


  void check_syntax() const {
    if (!process_command(QCMDCHK))
      throw std::runtime_error("Invalid command syntax");
  }


  public:
  command& operator=(const char* rhs) {
    m_command = rhs;
    auto begin = m_command.begin();
    std::transform(begin, m_command.end(), begin, toupper);
    check_syntax();
    return *this;
  }


  severity_t execute() const {
    process_command(QCMDEXC);
    return g_exception_info.Severity;
  }


  std::string name() const {
    auto end = m_command.find_first_of(BLANK);
    return m_command.substr(0, end);
  }


  std::string keyword_value(const std::string& keyword) const {
    auto search = keyword + "(";
    auto s  = m_command.find(search);
    if (s == std::string::npos)
      return std::string();

    s += search.length();
    auto e = m_command.find_first_of(")", s);
    auto result =  m_command.substr(s, e - s);

    s = result.find_first_not_of(BLANK);
    e = result.find_last_not_of(BLANK);
    return result.substr(s, e - s + 1);
  }
};

const std::string command::EVENTF_OPTION = "*EVENTF";

} // namespace oli { ///////////////////////////////////////////////////////////////////////////////////////////////////





//***** FILE_HANDLE CLASS DECLARATION **********************************************************************************
namespace detail {

class file_handle
{
  std::string m_mode;
  _RFILE*     m_handle;


  void open(const std::string& path) {
    m_handle = _Ropen(path.c_str(), m_mode.c_str());
    if (m_handle == nullptr)
      throw std::runtime_error(__FUNCTION__);
  }


  public:
  file_handle()
  : m_handle(nullptr) {}


  file_handle(const std::string& path, const std::string& mode)
  : m_mode(mode), m_handle(nullptr) {
    open(path);
  }


  file_handle(const file_handle& src)
  : m_mode(src.m_mode), m_handle(nullptr) {
    if (src.m_handle == nullptr) return;

    auto ofb = _Ropnfbk(src.m_handle);
    if (!ofb) throw std::runtime_error(__FUNCTION__);

    auto path = oli::get_string(ofb->library_name) + "/" +
                oli::get_string(ofb->file_name)    + "(" +
                oli::get_string(ofb->member_name)  + ")";

    open(path);
  }


  file_handle(file_handle&& src)
  : m_mode(src.m_mode), m_handle(src.m_handle) {
    src.m_handle = nullptr;
  }


  file_handle& operator=(const file_handle&) = delete;
  file_handle& operator=(file_handle&&) = delete;


  ~file_handle() {
    close();
  }


  void close() {
    if (!m_handle) return;
    _Rclose(m_handle);
    m_handle = nullptr;
  }


  _RFILE*& handle() {
    return m_handle;
  }


  bool operator==(const file_handle& rhs) const {
    return m_handle == nullptr && rhs.m_handle == nullptr;
  }
};

} // namespace detail { ////////////////////////////////////////////////////////////////////////////////////////////////





//***** ITERATOR CLASS DECLARATION *************************************************************************************
namespace detail {

template <typename R, typename K>
class iterator
{
  file_handle m_file;
  size_t      m_key_size;
  R           m_record;


  void check_end() {
    if (m_file.handle()->riofb.num_bytes != sizeof(R))
      m_file.close();
  }


  public:
  iterator()
  : m_file(), m_key_size(0) {}


  iterator(const file_handle& file)
  : m_file(file), m_key_size(0) {
    _Rreadf(m_file.handle(), &m_record, sizeof(R), __NO_LOCK);
    check_end();
  }


  iterator(const file_handle& file, K key, size_t key_size)
  : m_file(file), m_key_size(key_size) {
    _Rreadk(m_file.handle(), &m_record, sizeof(R), __DFT | __NO_LOCK, &key, m_key_size);
    check_end();
  }


  iterator(const iterator&) = delete;


  iterator(iterator&& src)
  : m_file(stl::move(src.m_file)), m_key_size(src.m_key_size), m_record(src.m_record) {}


  iterator& operator=(const iterator&) = delete;
  iterator& operator=(iterator&&) = delete;
  ~iterator() = default;


  bool operator==(const iterator& rhs) { return m_file == rhs.m_file; }
  bool operator!=(const iterator& rhs) { return !(*this == rhs); }
  R*   operator->()                    { return &m_record; }
  R&   operator*()                     { return m_record; }


  R& operator++() {
    if (m_key_size)
      _Rreadk(m_file.handle(), &m_record, sizeof(R), __KEY_NEXTEQ | __NO_LOCK,
              nullptr, m_key_size);
    else
      _Rreadn(m_file.handle(), &m_record, sizeof(R), __NO_LOCK);

    check_end();
    return m_record;
  }
};

} // namespace detail { ////////////////////////////////////////////////////////////////////////////////////////////////





//***** FILE CLASS DECLARATION *****************************************************************************************
namespace oli {

template <typename R, typename K = void>
class file
{
  static_assert(std::tr1::is_class<R>::value,
                "Type of Record must be a class");
  static_assert(std::tr1::is_class<K>::value || std::tr1::is_same<K, void>::value,
                "Type of Key must be a class or void");


  detail::file_handle m_file;


  public:
  typedef detail::iterator<R, K> iterator;


  file()
  : m_file() {}


  file(const std::string& path, const std::string& mode = "rr")
  : m_file(path, mode) {}


  iterator begin() {
    return iterator(m_file);
  }


  iterator end() {
    return iterator();
  }


  iterator find(const K& key, size_t key_size = sizeof(K)) {
    return iterator(m_file, key, key_size);
  }
};

} // namespace oli { ///////////////////////////////////////////////////////////////////////////////////////////////////





//***** CURRENT JOB CLASS DECLARATION **********************************************************************************
namespace oli {

class current_job
{
  Qwc_JOBI0700_t              m_job_info;
  std::vector<Qwc_Lib_List_t> m_libraries;


  void retrieve_job_info(void* buffer, size_t size) {
    istring<26> job_name("*");
    istring<16> job_id;
    Qus_EC_t error = { sizeof(Qus_EC_t) };

    QUSRJOBI(buffer, size, "JOBI0700", &job_name, &job_id, &error);

    if (error.Bytes_Available)
      throw std::runtime_error("Retrieve job information error");
  }


  public:
  current_job() {
    update();
  }


  void update() {
    retrieve_job_info(&m_job_info, sizeof(m_job_info));
    if (m_job_info.Bytes_Return == m_job_info.Bytes_Avail)
      return;

    std::vector<char> buffer(m_job_info.Bytes_Avail);
    retrieve_job_info(&buffer[0], buffer.size());
    std::memcpy(&m_job_info, &buffer[0], sizeof(m_job_info));

    m_libraries.resize(m_job_info.Libs_In_Syslibl + m_job_info.Prod_Libs +
                       m_job_info.Curr_Libs + m_job_info.Libs_In_Usrlibl);
    std::memcpy(&m_libraries[0], &buffer[sizeof(m_job_info)],
                buffer.size() - sizeof(m_job_info));
  }


  std::string current_library() const {
    if (!m_job_info.Curr_Libs)
      return std::string();

    auto idx = m_job_info.Libs_In_Syslibl + m_job_info.Prod_Libs;
    return get_string(m_libraries[idx].Lib_Name);
  }
};

} // namespace oli { ///////////////////////////////////////////////////////////////////////////////////////////////////


#endif

