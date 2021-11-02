// =================================================================
//
//  MODULE:    TMKMSGHD
//
//
//  MODULE-TYPE: C Library
//
//  Processor:  C
//
//  Purpose:    Message handling module
//
// =================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "base"
#include "msghandle"
#include "sysapi"
#include "option"


// =================================================================
//  Variables definitions
// =================================================================
Static Boolean usrmsg_to_session = FALSE;
Static error_rtn errrtn = {0};
Static Char msg_key[4];
Static Char msg_txt[MSG_TXT_SZ + MSG_LINE_NO_SZ];


// =================================================================
//  Function:    set_usrmsg_to_session ()
// =================================================================
Void set_usrmsg_to_session(Void)
{
  usrmsg_to_session = TRUE;
}


// =================================================================
//  Function:    get_usrmsg_to_session ()
// =================================================================
Boolean get_usrmsg_to_session(Void)
{
  return (usrmsg_to_session);
}


// =================================================================
//  Function:    log_error_msgt ()
// =================================================================
Static
Void
log_error_msgt(Char *msgid, Char *txt, Int32 line_no, Char* msgt)
{
  Char *msg = msg_txt;
  Int32 msg_sz = 0;
  Int32 tmp_sz;

  if (line_no > 0)
  {
    // setup the &1 if the message has line no information
    sprintf(msg_txt, "%5d", line_no);
    msg = msg_txt + 5;
    msg_sz = 5;
  }
  tmp_sz = (txt == NULL) ? 0 : strlen(txt);
  if (tmp_sz != 0)
  {
    // setup the rest of &2, &3.... parameters
    tmp_sz = Min(MSG_TXT_SZ, tmp_sz);
    memcpy(msg, txt, tmp_sz);
    msg_sz += tmp_sz;
  }

  QMHSNDPM(msgid, MSGF_PATH, msg_txt, msg_sz, msgt,
           "*         ", 0, msg_key, (char *)&errrtn);
}


// =================================================================
//  Function:    log_error_and_exit ()
// =================================================================
Void log_error_and_exit(Char *msgid, Char *txt, Int32 line_no)
{
  Char* msgt = opt_except() ? MSGT_ESCAPE : MSGT_COMPLETE;
  log_error_msgt(msgid, txt, line_no, msgt);
  exit(TMK_EXIT_FAILURE);
}


// =================================================================
//  Function:    log_error ()
// =================================================================
Void log_error(Char *msgid, Char *txt, Int32 line_no)
{
  log_error_msgt(msgid, txt, line_no, MSGT_COMPLETE);
}


// =================================================================
//  Function:    log_usrmsg ()
// =================================================================
Void log_usrmsg(Char *txt)
{
  if (usrmsg_to_session)
  {
    fprintf(stdout, "%s\n", txt);
  }
  else
  {
    log_error(USER_CMD, txt, MSG_NO_LINE_NO);
  }
}


// =================================================================
//  Function:    log_dbg ()
// =================================================================
Void log_dbg(Char *txt)
{
  fprintf(stdout, "%s\n", txt);
}

