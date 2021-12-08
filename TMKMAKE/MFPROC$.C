// =================================================================
//
//  MODULE:    TMKMAKER
//
//
//  MODULE-TYPE: C Library
//
//  Processor:  C
//
//  Purpose:
//              Makefile processor
//
// =================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#ifdef __ILEC400__
#include <milib.h>
#endif

#include "base"
#include "mfproc"
#include "utility"
#include "file"
#include "parser"
#include "diction"
#include "builtin"
#include "option"
#include "sysapi"
#include "msghandle"
#include "eventf"

#ifdef __ILEC400__
#define __rtvlurc() (_LU_WORK_AREA->LU_RC)
#else
short __rtvlurc(void);
#endif //@01

//**********************************************************************
//        Variable declarations
//**********************************************************************
Static Boolean cmd_is_make;

// =================================================================
//  Function:    set_proc_date
// =================================================================
Static void
set_proc_date(File_spec_t *tfs, File_spec_t *dfs)
{
  if (cmp_date(tfs->proc_update, dfs->last_update) < 0)
  {
    tfs->proc_update.val = dfs->last_update.val;
  }

  if (cmp_date(tfs->proc_update, dfs->proc_update) < 0)
  {
    tfs->proc_update.val = dfs->proc_update.val;
  }
}

// =================================================================
//  Function:    target_older
//  Description: To determine if one or more of the dependent(s)
//               is(are) more recent than the target
//  update last_ep field in t & d
//  Input:
//  Output:
// =================================================================
Static
    Boolean
    target_older(Element_t *t, Element_t *d, Int16 line, Date_t last_update)
{
  Element_t *ep;
  Boolean rc = FALSE;

#ifdef SRVOPT
  if (srvopt_function())
  {
    sprintf(srv_cat, "FCT:target_older(%s),", srv_e(t));
    printf("%s%s)\n", srv_cat, srv_e(d));
  }
#endif

  // if target does not exists
  if (t->fs.last_update.val == 0)
  {
#ifdef SRVOPT
    if (srvopt_fctrtn())
      printf("RTN:target_older:1\n");
#endif
    return (TRUE);
  }

  // Target does exist.
  // If no dependent, never execute dependency rules
  if (d == NULL)
  {
#ifdef SRVOPT
    if (srvopt_fctrtn())
      printf("RTN:target_older:0\n");
#endif
    return (FALSE);
  }

  // compare all dependents with target date
  while (d)
  {
    update_file_date(&d->fs, line);
    ep = d;
    d = d->nxt;

    if ((cmp_date((last_update.val == 0 ? t->fs.last_update : last_update),
                  ep->fs.last_update) < 0) ||
        (cmp_date((last_update.val == 0 ? t->fs.last_update : last_update),
                  ep->fs.proc_update) < 0))
    {
      set_proc_date(&t->fs, &ep->fs);
      rc = TRUE;
    }
    // check for the same target/dependent program object.
    // exception for PGM of the same name in target and
    // dependent is to check last_update_date and create_date
    if (strcmp(t->fs.type, FS_T_PGMSET) == 0 &&
        strcmp(ep->fs.type, FS_T_PGM) == 0 &&
        strcmp(t->fs.lib, ep->fs.lib) == 0 &&
        strcmp(t->fs.file, ep->fs.file) == 0)
    {
      // target and dependent are the same *PGM obj
      if (cmp_date((last_update.val == 0 ? t->fs.last_update : last_update), t->fs.create_date) == 0)
      {
        rc = TRUE;
      }
    }
  }
#if 0
        // update all dates in dependent list for later use
        while( d ) {
            update_file_date( &d->fs, line );
            d       = d->nxt;
        }
#endif
#ifdef SRVOPT
  if (srvopt_fctrtn())
    printf("RTN:target_older:%d\n", rc);
#endif
  return (rc);
}

// =================================================================
//  Function:    make_in_command ()
// =================================================================
Static
    Boolean
    make_in_command(Char *cmd)
{
  Char *cp;
  Char *s;
  Char save;
  Boolean rc;

#ifdef SRVOPT
  if (srvopt_function())
    printf("FCT:make_in_command(\"%s\")\n", cmd);
#endif
  // set up to only search for the first word in cmd
  cp = skip_non_white_spaces(cmd);
  save = *cp; // save char at null terminated position
  *cp = 0;    // null terminated the first word for strstr

  s = strchr(cmd, '/');
  rc = (strcmp((s == NULL) ? cmd : s + 1, "TMKMAKE") == 0);

  *cp = save; // restore character at null position

#ifdef SRVOPT
  if (srvopt_fctrtn())
    printf("RTN:make_in_command:%d\n", rc);
#endif
  return (rc);
}

// =================================================================
//  Function:    create_nested_make_cmd ()
// =================================================================
Static
    Char *
    create_nested_make_cmd(Char *cmd, Buf_t *b)
{
  Char *tp;
  Char save;

#ifdef SRVOPT
  if (srvopt_function())
    printf("FCT:create_nested_make_cmd(\"%s\",Buf_t *b)\n", cmd);
#endif
  reset_buf(b);

  // look for the option( keyword in command to determine
  // whether to add the OPTION() or add to existing option
  if ((tp = strstr_no_case(cmd, "OPTION(")) == NULL)
  {
    // determine existence of the 5th (i.e. option) positional
    // parameter. e.g. strmake *first *lib/qmaksrc *first
    append_buf(b, cmd);
    append_buf(b, " OPTION(*NOEXEC)");
  }
  else
  {
    // OPTION parameter found, insert the *NOEXEC keyword
    // in option.
    save = tp[7];
    tp[7] = 0;
    append_buf(b, cmd);
    append_buf(b, "*NOEXEC ");
    tp[7] = save;
    append_buf(b, tp + 7);
  }
#ifdef SRVOPT
  if (srvopt_fctrtn())
    printf("RTN:create_nested_make_cmd:\"%s\"\n", b->bp);
#endif
  return (b->bp);
}


// =================================================================
//  Function:    exec_command ()
// =================================================================
Static
Int16
exec_command(Char* cmd, Int16 line)
{
  // No need execute command
  if (opt_no_execute())
  {
    // Command is no 'TMKMAKE'
    if (!cmd_is_make)
      return 0;

    // Propagate the opt_no_execute to nested make
    cmd = create_nested_make_cmd(cmd, &t1);
  }

  // Check for command text too long
  if (strlen(cmd) > QCMDEXC_MAX_SZ)
    log_error_and_exit(CMD_EXCEEDED_3000, NULL, line);

  // Execute command and processing result
  Int16 severity = eventf_execute_command(cmd);

  if (severity == EXEC_INVALID_CMD)
    log_error_and_exit(INV_CMD_SYNTAX, NULL, line);
  if (severity == EXEC_OBJLIB_EMPTY)
    log_error_and_exit(INV_OBJLIB_EMPTY, NULL, line);

  if (severity == 0 &&
      (cmd_is_make || opt_get_rtncde_methods() != RTN_EXCEPTION))
    return __rtvlurc();

  return severity;
}


// =================================================================
//  Function:    set_pre_defined_macros ()
// =================================================================
//**********************************************************************
// Re-define the following pre-defined macros for the current rule
//        $?,$**,$%
//        $@,$(@L),$(@F),$(@M),$(@T),$(@S)
//        $*,$(*L),$(*F),$(*M),$(*T),$(*S)
//        $<,$(<L),$(<F),$(<M),$(<T),$(<S)
//**********************************************************************
Static
    Void
    set_pre_defined_macros(Rules_t *rp)
{
  Element_t *ep;
  Date_t target_date;
  Char *cp;
  Boolean is_file = rp->target->fs.is_file;

#ifdef SRVOPT
  if (srvopt_function())
    printf("FCT:set_pre_defined_macro(%s)\n", srv_rule(rp));
#endif
  // build $@ macros list
  update_sym("@", rp->target->name, FALSE);
  update_sym("@L", rp->target->fs.lib, FALSE);
  update_sym("@F", rp->target->fs.file, FALSE);
  update_sym("@M", rp->target->fs.extmbr, FALSE);
  update_sym("@T", rp->target->fs.type, FALSE);
  update_sym("@S", rp->target->fs.seu_type, FALSE);

  // build $% macros list each time $@ is evaluated
  update_sym("%",
             ((strcmp(rp->target->fs.type, FS_T_LIBFILE) == 0 ||
               strcmp(rp->target->fs.type, FS_T_TXTLIB) == 0 ||
               strcmp(rp->target->fs.type, FS_T_BNDDIR) == 0) &&
              *rp->target->fs.extmbr != 0)
                 ? rp->target->fs.extmbr
                 : "",
             FALSE);

  // build updated dependency $? & $** macros list
  ep = rp->dependent;
  target_date = rp->target->fs.last_update;
  reset_buf(&t1);
  reset_buf(&t2);

  // loop through all the dependent in list to build $? & $**
  while (ep)
  {
    append_buf(&t2, ep->name);
    append_buf(&t2, " ");

    if (cmp_date(target_date, ep->fs.last_update) < 0)
    {
      append_buf(&t1, ep->name);
      append_buf(&t1, " ");
    }
    ep = ep->nxt;
  }
  // adjust buffer and get rid of last blanks in expanded text
  if (t1.bp != t1.tp)
  {
    --t1.tp;
    *t1.tp = 0;
  }
  if (t2.bp != t2.tp)
  {
    --t2.tp;
    *t2.tp = 0;
  }

  update_sym("?", t1.bp, FALSE);
  update_sym("**", t2.bp, FALSE);

  // build $< macros list
  if (rp->implicit_rule)
  {
    // build $* & $< macros list for implicit rule only
    reset_buf(&t1);
    append_buf(&t1, cp = rp->target->fs.lib);
    append_buf(&t1, *cp ? "/" : "");
    append_buf(&t1, is_file ? rp->target->fs.extmbr
                            : rp->target->fs.file);
    update_sym("*", t1.bp, FALSE);

    update_sym("*L", rp->target->fs.lib, FALSE);
    update_sym("*F", rp->target->fs.file, FALSE);
    update_sym("*M", rp->target->fs.extmbr, FALSE);
    update_sym("*T", rp->target->fs.type, FALSE);
    update_sym("*S", rp->target->fs.seu_type, FALSE);

    // rule may not have dependents e.g. .DEFAULT
    if (rp->dependent)
    {
      update_sym("<", rp->dependent->name, FALSE);
      update_sym("<L", rp->dependent->fs.lib, FALSE);
      update_sym("<F", rp->dependent->fs.file, FALSE);
      update_sym("<M", rp->dependent->fs.extmbr, FALSE);
      update_sym("<T", rp->dependent->fs.type, FALSE);
      update_sym("<S", rp->dependent->fs.seu_type, FALSE);
    }
    else
    {
      update_sym("<", "", FALSE);
      update_sym("<L", "", FALSE);
      update_sym("<F", "", FALSE);
      update_sym("<M", "", FALSE);
      update_sym("<T", "", FALSE);
      update_sym("<S", "", FALSE);
    }
  }
  else
  {
    // no $* & $< macros text for explicit rule
    update_sym("*", "", FALSE);
    update_sym("*L", "", FALSE);
    update_sym("*F", "", FALSE);
    update_sym("*M", "", FALSE);
    update_sym("*T", "", FALSE);
    update_sym("*S", "", FALSE);

    update_sym("<", "", FALSE);
    update_sym("<L", "", FALSE);
    update_sym("<F", "", FALSE);
    update_sym("<M", "", FALSE);
    update_sym("<T", "", FALSE);
    update_sym("<S", "", FALSE);
  }
}

// =================================================================
//  Function:    get_cmd_sev ()
// =================================================================
Static
    Int32
    get_cmd_sev(Char **cmd)
{
  Int32 rc = 0;
  Char *cp = *cmd;

#ifdef SRVOPT
  if (srvopt_function())
    printf("FCT:get_cmd_sev(%s)\n", *cmd);
#endif
  // convert all digits into a binary variable
  while (isdigit(*cp))
  {
    rc *= 10;
    rc += *cp - '0';
    ++cp;
  }
  *cmd = cp;
  // if severity is specified, it must be followed by a white
  // space character, otherwise return -1 (unsuccessful)
  rc = isspace(*cp) ? rc : -1;

#ifdef SRVOPT
  if (srvopt_fctrtn())
    printf("RTN:get_cmd_sev:%d\n", rc);
#endif
  return (rc);
}

// =================================================================
//  Function:    make_target ()
// =================================================================
Static
    Boolean
    make_target(Rules_t *rp)
{
  Element_t *ep;
  Rules_t *drp;
  Boolean rc = FALSE;
  Boolean target_origin_date_set = FALSE;
  Date_t rp_last_update;

#ifdef SRVOPT
  if (srvopt_function())
    printf("FCT:make_target(%s)\n", srv_rule(rp));
#endif
  if (rp->recursive)
  {
    // recursive target processing is detected
    log_error(RECURSIVE_TARGET, rp->target->name, rp->line);
    return (rc);
  }

  rp_last_update.val = 0;

  // update target object last changed date
  update_file_date(&rp->target->fs, rp->line);

  // loop for all targets of the same name in rule list
  //  i.e. target_name:: rules
  while (rp != NULL)
  {
    rp->recursive = TRUE;

    // make sure all the dependents are updated before make
    // current rules
    ep = rp->dependent;
    while (ep)
    {
      ep->maked = FALSE;
      if ((drp = in_rule_list(ep->name)) != NULL)
      {
        // dependent file either in explicit or
        //  or implicit rule spec.
        ep->maked |= make_target(drp);
        rp->dependent->fs.proc_update.val =
            drp->target->fs.proc_update.val;
      }

      // update the dependent date, this must exist
      if (!ep->maked && !obj_exists(&ep->fs, rp->line))
      {
        log_error_and_exit(CANT_PROC_TARGET, ep->name, rp->line);
      }
      ep = ep->nxt;
    }

    // if touch option is specified, touch the target file
    if (opt_touch_target())
    {
      touch_target(rp->target);
    }
    else
    {
      if (target_older(rp->target, rp->dependent,
                       rp->line, rp_last_update))
      {

        // target older than any one of the dependent(s)
        //  execute all the command in the command list
        Cmd_t *cur_cmd = rp->cmd;
        // set pre-defined macros:
        set_pre_defined_macros(rp);
        if (opt_debug())
        {
          dump_dict();
        }

        // execute all the command specified for current rule
        while (cur_cmd != NULL)
        {
          Boolean no_echo = FALSE;
          Boolean no_err_stop = FALSE;
          Char echo_str[5];
          Char *cmd = cur_cmd->cmd_txt;
          Int32 cmd_sev = opt_get_rtncde_sev();

          // process the @ and - prefixes for each cmd
          while (*cmd == '@' || *cmd == '-')
          {
            if (*cmd == '@')
              no_echo = TRUE;

            else if (*cmd == '-')
            {
              no_err_stop = TRUE;
              // look for severity level after -
              if (isdigit(cmd[1]))
              {
                ++cmd;
                cmd_sev = get_cmd_sev(&cmd);
                no_err_stop = FALSE;
                if (cmd_sev < 0)
                {
                  // invalid value specified or
                  // no space after digits
                  log_error_and_exit(INV_PREFIX_RTNSEV, NULL, cur_cmd->line);
                }
              }
              else
                cmd_sev = ~(1 << ((sizeof(cmd_sev) * 8) - 1));
            }

            ++cmd;
          }

          cmd = skip_white_spaces(cmd);
          cmd_is_make = (opt_no_execute() ? make_in_command(cmd) : FALSE);

          // expand text substitution(s) if needed
          // of all the built-in macros
          cmd = bi_macro_substitution(cmd);

          // echo command line
          if ((!no_echo && !opt_silent_mode()) || opt_no_execute())
            log_usrmsg(cmd);

          // check if special echo cmd and process
          strncpy(echo_str, cmd, 4);
          echo_str[4] = 0;
          to_upper_str(echo_str);
          if (strncmp(echo_str, "ECHO", 4) == 0)
          {
            // process built-in command "echo"
            if (!opt_no_execute())
            {
              cmd = skip_white_spaces(cmd + 4);
              log_usrmsg(cmd);
            }
          }
          else
          {
            // execute command
            Int16 rc = exec_command(cmd, cur_cmd->line);
            if (rc >= cmd_sev)
            {
              // error/exception occurs, handle
              // depends on flags set
              if (!no_err_stop && !opt_ignore_err_code())
                log_error_and_exit(CMD_FAILED, NULL, cur_cmd->line);
              else
                log_error(CMD_FAILED, NULL, cur_cmd->line);
            }
          }
          cur_cmd = cur_cmd->nxt;
        }
        rc = TRUE;
      }
    }
    // reset make recursion prevention flag
    rp->recursive = FALSE;

    // only multiple rules :: are allowed to repeat for the
    // same target name (i.e. library maintenance rules)
    if (rp->op == OP_MULTI_RULES)
    {
      Rules_t *crp;
      // set up target original update date
      if (!target_origin_date_set)
      {
        target_origin_date_set = TRUE;
        rp_last_update.val = rp->target->fs.last_update.val;
      }
      // looks for any other rule in list which have the
      // same same name, if so make the new target
      crp = rp;
      rp = crp->nxt;
      while (rp)
      {
        if (strcmp(rp->target->name,
                   crp->target->name) == 0)
        {
          break;
        }
        rp = rp->nxt;
      }
    } // if( rp->op == OP_MULTI_RULES )
    else
    {
      rp = NULL;
    }
  } // while( rp != NULL )
  return (rc);
}

// =================================================================
//  Function:    process_makefile ()
// =================================================================
Void process_makefile(Void)
{
  Rules_t *make_rule;

#ifdef SRVOPT
  if (srvopt_function())
    printf("FCT:process_makefile(Void)\n");
#endif
  set_make_state();
  dump_builtins();
  dump_dict();
  dump_rule_list();

  // get the first target specified in the STRMAKE command
  make_rule = get_first_applied_target();

  while (make_rule)
  {
    // make the target and its dependents recursively
    make_target(make_rule);
    // log message about the target is up-to-date
    log_error(TARGET_UP_TO_DATE, make_rule->target->name, make_rule->line);
    // get next target specified in the STRMAKE command
    make_rule = get_next_applied_target();
  }
}
