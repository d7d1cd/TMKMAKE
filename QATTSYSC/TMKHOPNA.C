/* ================================================================= */
/*                                                                   */
/*  MODULE:    TMKHOPNA                                              */
/*                                                                   */
/*                                                                   */
/*  MODULE-TYPE: C Library                                           */
/*                                                                   */
/*  Processor:  C                                                    */
/*                                                                   */
/*  Purpose:    Makefile processing functions - header file          */
/*                                                                   */
/* ================================================================= */
 
#ifndef _TMKHOPNA_H
#define _TMKHOPNA_H
 
#ifdef __ILEC400__
   #include <except.h>
#endif
 
/*********************************************************************/
/*  PROTOTYPES FOR CALLING OPENNESS MODULES                          */
/*********************************************************************/
 
#pragma linkage (QUSCRTUS, OS)
void QUSCRTUS(char *, char *, int, char *, char *, char *, ...);
 
#pragma linkage (QUSRTVUS, OS)
void QUSRTVUS(char *, int, int, char *, ...);
 
#pragma linkage (QUSCHGUS, OS)
void QUSCHGUS(char *, int, int, char *, char *, ...);
 
#pragma linkage (QUSPTRUS, OS)
void QUSPTRUS(char *, void *, ...);
 
#pragma linkage (QUSCRTUQ, OS)
void QUSCRTUQ(char *, char *, char *, int, int,
              int, int, char *, char *, ...);
 
#pragma linkage (QUSCRTUI, OS)
void QUSCRTUI(char *, char *, char *, int, char *, int,
              char *, char *, char *, char *, ...);
 
#pragma linkage (QUSLOBJ, OS)
void QUSLOBJ(char *, char *, char *, char *, ...);
 
#pragma linkage (QUSROBJD, OS)
void QUSROBJD(char *, int, char *, char *, char *, ...);
 
#pragma linkage (QUSLMBR, OS)
void QUSLMBR(char *, char *, char *, char *, char *, ...);
 
#pragma linkage (QUSRMBRD, OS)
void QUSRMBRD(char *, int, char *, char *, char *, char *, ...);
 
#pragma linkage (QUSLRCD, OS)
void QUSLRCD(char *, char *, char *, char *, ...);
 
#pragma linkage (QUSLFLD, OS)
void QUSLFLD(char *, char *, char *, char *, char *, ...);
 
#pragma linkage (QUSLSPL, OS)
void QUSLSPL(char *, char *, char *, char *, char *, char *, ...);
 
#pragma linkage (QUSRSPLA, OS)
void QUSRSPLA(char *, int, char *, char *, char *, char *,
              char *, int, ...);
 
#pragma linkage (QUSLJOB, OS)
void QUSLJOB(char *, char *, char *, char *, ...);
 
#pragma linkage (QUSRJOBI, OS)
void QUSRJOBI(char *, int, char *, char *, char *, ...);
 
#pragma linkage (QUSCHGPA, OS)
void QUSCHGPA(int, int, int, ...);
 
#pragma linkage (QPRCRTPG, OS)
void QPRCRTPG(char *, int, char *, char *, char *, char *,
              char *, char *, int, char *, char *, int, ...);
 
#pragma linkage (QUSDLTUI, OS)
void QUSDLTUI(char *, void *);
 
#pragma linkage (QUSDLTUQ, OS)
void QUSDLTUQ(char *, void *);
 
#pragma linkage (QUSDLTUS, OS)
void QUSDLTUS(char *, void *);
 
#pragma linkage (QMHSNDPM, OS)
void QMHSNDPM(char *, char *, char *, int, char *, char*, int,
              char *, char *);
 
#define MSGT_COMPLETE                 "*COMP     "
#define MSGT_DIAG                     "*DIAG     "
#define MSGT_ESCAPE                   "*ESCAPE   "
#define MSGT_INFO                     "*INFO     "
#define MSGT_INQ                      "*INQ      "
#define MSGT_NOTIFY                   "*NOTIFY   "
#define MSGT_RQS                      "*RQS      "
#define MSGT_STATUS                   "*STATUS   "
 
typedef struct error_rtn {
     int  rtn_area_sz;
     int  exception_area_sz;
     char exception_idы7Е;
     char reserve;
     char exception_dataы1Е;
} error_rtn;
 
#ifdef __ILEC400__
  #pragma linkage (QMHCHGEM, OS)
  void QMHCHGEM ( _INVPTR *, int, unsigned int, char *,
                 char *, int, error_rtn * );
#endif
 
#define MOD_HANDLE               "*HANDLE   "
#define MOD_RMVLOG               "*REMOVE   "
#define MOD_REPLY                "*REPLY    "
 
/********************************************************************/
/*  Generic header returned in userspaces following a call to       */
/*  a "LIST" module.                                                */
/********************************************************************/
 
typedef struct header_struct {
     char user_dataы64Е;
     int  generic_header_size;
     char header_versionы4Е;
     char format_nameы8Е;
     char program_nameы10Е;
     char time_generatedы13Е;
     char information_status;
     int  usrspc_used;
     int  parm_section_offset;
     int  parm_section_size;
     int  header_section_offset;
     int  header_section_size;
     int  list_section_offset;
     int  list_section_size;
     int  number_of_entries;
     int  size_of_entry;
} header_struct;
 
/********************************************************************/
/*  Structures returned by the QUSROBJD API                         */
/********************************************************************/
 
typedef struct D0100FMT {
                 int bytes_returned;
                 int bytes_available;
                 char object_nameы10Е;
                 char object_libraryы10Е;
                 char object_typeы10Е;
                 char return_libraryы10Е;
                 int asp_number;
                 char object_ownerы10Е;
                 char object_domainы2Е;
                 char creation_dateы13Е;
                 char change_dateы13Е;
               } D0100FMT;
 
typedef struct D0200FMT {
                 int bytes_returned;
                 int bytes_available;
                 char object_nameы10Е;
                 char object_libraryы10Е;
                 char object_typeы10Е;
                 char return_libraryы10Е;
                 int asp_number;
                 char object_ownerы10Е;
                 char object_domainы2Е;
                 char creation_dateы13Е;
                 char change_dateы13Е;
                 char extended_attributeы10Е;
                 char oirtextы50Е;
                 char source_fileы10Е;
                 char source_file_libы10Е;
                 char source_file_mbrы10Е;
               } D0200FMT;
 
typedef struct D0300FMT {
                 int bytes_returned;
                 int bytes_available;
                 char object_nameы10Е;
                 char object_libraryы10Е;
                 char object_typeы10Е;
                 char return_libraryы10Е;
                 int asp_number;
                 char object_ownerы10Е;
                 char object_domainы2Е;
                 char creation_dateы13Е;
                 char change_dateы13Е;
                 char extended_attributeы10Е;
                 char oirtextы50Е;
                 char source_fileы10Е;
                 char source_file_libы10Е;
                 char source_file_mbrы10Е;
                 char source_file_updateы13Е;
                 char savedateы13Е;
                 char restore_dateы13Е;
                 char creating_userprfы10Е;
                 char system_created_onы8Е;
                 char reset_dateы7Е;
                 int save_size;
                 int save_sequence_number;
                 char storageы10Е;
                 char save_commandы10Е;
                 char save_volidы71Е;
                 char save_deviceы10Е;
                 char save_fileы10Е;
                 char save_file_libы10Е;
                 char save_labelы17Е;
                 char system_levelы9Е;
                 char compilerы16Е;
                 char object_levelы8Е;
                 char user_change;
                 char licensed_programы16Е;
                 char ptfы10Е;
                 char aparы10Е;
               } D0300FMT;
 
typedef struct D0400FMT {
                 int bytes_returned;
                 int bytes_available;
                 char object_nameы10Е;
                 char object_libraryы10Е;
                 char object_typeы10Е;
                 char return_libraryы10Е;
                 int asp_number;
                 char object_ownerы10Е;
                 char object_domainы2Е;
                 char creation_dateы13Е;
                 char change_dateы13Е;
                 char extended_attributeы10Е;
                 char oirtextы50Е;
                 char source_fileы10Е;
                 char source_file_libы10Е;
                 char source_file_mbrы10Е;
                 char source_file_updateы13Е;
                 char savedateы13Е;
                 char restore_dateы13Е;
                 char creating_userprfы10Е;
                 char system_created_onы8Е;
                 char reset_dateы7Е;
                 int save_size;
                 int save_sequence_number;
                 char storageы10Е;
                 char save_commandы10Е;
                 char save_volidы71Е;
                 char save_deviceы10Е;
                 char save_fileы10Е;
                 char save_file_libы10Е;
                 char save_labelы17Е;
                 char system_levelы9Е;
                 char compilerы16Е;
                 char object_levelы8Е;
                 char user_change;
                 char licensed_programы16Е;
                 char ptfы10Е;
                 char aparы10Е;
                 char last_use_dateы7Е;
                 char update_object_usage;
                 int  use_count;
                 int object_size;
                 int object_multiplier;
                 char compress;
               } D0400FMT;
 
/********************************************************************/
/*  Structures returned by the QUSLOBJ API                          */
/********************************************************************/
 
typedef struct lobj_input_parms {
                 char user_space_nameы10Е;
                 char user_space_libraryы10Е;
                 char format_nameы8Е;
                 char object_name_specifiedы10Е;
                 char library_name_specifiedы10Е;
                 char object_type_specifiedы10Е;
               } lobj_input_parms;
 
typedef struct lobj_header_section {
                 char object_name_specifiedы10Е;
                 char library_name_specifiedы10Е;
                 char object_type_specifiedы10Е;
               } lobj_header_section;
 
typedef struct OBJL0100 {
                 char object_nameы10Е;
                 char library_nameы10Е;
                 char object_typeы10Е;
               } OBJL0100;
/********************************************************************/
/*  Structures returned by the QUSLJOB API                          */
/********************************************************************/
 
typedef struct ljob_input_parms {
                 char job_name_specifiedы10Е;
                 char user_name_specifiedы10Е;
                 char job_number_specifiedы6Е;
                 char status_specifiedы10Е;
               } ljob_input_parms;
 
typedef struct ljob_header_section {
                 char job_name_searched_forы10Е;
                 char user_name_searched_forы10Е;
                 char job_number_searched_forы6Е;
               } ljob_header_section;
 
typedef struct JOBL0100 {
                 char job_nameы10Е;
                 char user_nameы10Е;
                 char job_numberы6Е;
                 char internal_job_idы16Е;
                 char statusы10Е;
               } JOBL0100;
/*******************************************************************/
/* Input structure for QUSLMBR header section                      */
/*******************************************************************/
typedef struct mbrl_input {
     char user_spaceы20Е;
     char formatы8Е;
     char file_and_libы20Е;
     char member_nameы10Е;
     char override;
} mbrl_input;
 
/*******************************************************************/
/* Record structure for QUSLMBR header section                     */
/*******************************************************************/
typedef struct mbrl_header {
     char file_nameы10Е;
     char file_libы10Е;
     char file_attrы10Е;
     char file_txtы50Е;
     int  tot_members;
     char source_file;
} mbrl_header;
 
 
/*******************************************************************/
/* Record structure for QUSLMBR MBRL0100 format                    */
/*******************************************************************/
typedef struct mbrl0100 {
     char member_nameы10Е;
} mbrl0100;
 
/*******************************************************************/
/* Record structure for QUSLMBR MBRL0200 format                    */
/*******************************************************************/
typedef struct mbrl0200 {
     char member_nameы10Е;
     char source_typeы10Е;
     char crt_dateы13Е;
     char change_dateы13Е;
     char text_descы50Е;
} mbrl0200;
 
/*******************************************************************/
/* Record structure for QUSRMBRD MBRD0100 format                   */
/*******************************************************************/
typedef struct mbrd0100 {
     int  bytes_returned;
     int  bytes_available;
     char db_file_nameы10Е;
     char db_file_libы10Е;
     char member_nameы10Е;
     char file_attrы10Е;
     char src_typeы10Е;
     char crt_dateы13Е;
     char change_dateы13Е;
     char text_descы50Е;
     char src_file;
} mbrd0100;
 
 
/*******************************************************************/
/* Record structure for QUSRMBRD MBRD0200 format                   */
/*******************************************************************/
typedef struct mbrd0200 {
     int  bytes_returned;
     int  bytes_available;
     char db_file_nameы10Е;
     char db_file_libы10Е;
     char member_nameы10Е;
     char file_attrы10Е;
     char src_typeы10Е;
     char crt_dateы13Е;
     char src_change_dateы13Е;
     char text_descы50Е;
     char src_file;
     char ext_file;
     char log_file;
     char OPD_share;
     char RESERVEDы2Е;
     int  num_cur_rec;
     int  num_dlt_rec;
     int  dat_spc_size;
     int  acc_pth_size;
     int  num_dat_mbr;
     char change_dateы13Е;
     char save_dateы13Е;
     char rest_dateы13Е;
     char exp_dateы13Е;
     int  nbr_days_user;
     char date_lst_usedы7Е;
     char use_reset_dateы7Е;
} mbrd0200;
 
/*******************************************************************/
/* Input structure for QUSLRCD header section                      */
/*******************************************************************/
typedef struct rcdl_input {
     char user_space_nameы20Е;
     char format_nameы8Е;
     char file_name_and_libraryы20Е;
     char override;
} rcdl_input;
 
/*******************************************************************/
/* Record structure for QUSLRCD header section                     */
/*******************************************************************/
typedef struct rcdl_header {
     char file_nameы10Е;
     char file_libы10Е;
     char file_typeы10Е;
     char file_txt_descы50Е;
} rcdl_header;
 
/*******************************************************************/
/* Record structure for QUSLRCD RCDL0100 format                    */
/*******************************************************************/
typedef struct rcdl0100 {
     char record_fmt_nameы10Е;
} rcdl0100;
 
/*******************************************************************/
/* Record structure for QUSLRCD RCDL0200 format                    */
/*******************************************************************/
typedef struct rcdl0200 {
     char record_fmt_nameы10Е;
     char record_fmt_IDы13Е;
     char RESERVED1;
     int  record_length;
     int  number_fields;
     char rcrd_txt_descы50Е;
} rcdl0200;
 
/*******************************************************************/
/* Input structure for QUSLFLD header section                      */
/*******************************************************************/
typedef struct fldl_input {
     char space_nameы10Е;
     char space_libы10Е;
     char formatы8Е;
     char file_nameы10Е;
     char file_libы10Е;
     char format_format_nameы10Е;
     char override;
} fldl_input;
 
/*******************************************************************/
/* Record structure for QUSLFLD header section                     */
/*******************************************************************/
typedef struct fldl_header {
     char file_nameы10Е;
     char file_libы10Е;
     char file_typeы10Е;
     char rcd_fmt_nameы10Е;
     int  rcd_length;
     char record_fmt_IDы13Е;
     char rcrd_txt_descы50Е;
} fldl_header;
 
 
/*******************************************************************/
/* Record structure for QUSLFLD FLDL0100 format                    */
/*******************************************************************/
typedef struct fldl0100 {
     char field_nameы10Е;
     char data_type;
     char usage;
     int  output_buff_pos;
     int  input_buff_pos;
     int  field_length;
     int  field_digits;
     int  field_dec_pos;
     char field_descы50Е;
     char edit_codeы2Е;
     int  edit_wrd_lngth;
     char edit_wordы64Е;
     char col_head_1ы20Е;
     char col_head_2ы20Е;
     char col_head_3ы20Е;
} fldl0100;
 
 
/*******************************************************************/
/* Input structure for QUSLSPL header section                      */
/*******************************************************************/
typedef struct splf_input {
     char user_spaceы20Е;
     char format_nameы8Е;
     char user_nameы10Е;
     char outq_name_and_libы20Е;
     char form_typeы10Е;
     char user_dataы10Е;
} splf_input;
 
/*******************************************************************/
/* Record structure for QUSLSPL header section                     */
/*******************************************************************/
typedef struct splf_header {
     char usr_nameы10Е;
     char outq_nameы10Е;
     char outq_libы10Е;
} splf_header;
 
 
/*******************************************************************/
/* Record structure for QUSLSPL SPLF0100 format                    */
/*******************************************************************/
typedef struct splf0100 {
     char usr_nameы10Е;
     char outq_nameы10Е;
     char outq_libы10Е;
     char form_typeы10Е;
     char usr_dataы10Е;
     char int_job_IDы16Е;
     char int_splf_IDы16Е;
} splf0100;
 
 
/*******************************************************************/
/* Record structure for QUSRSPLA SPLA0100 format                   */
/*******************************************************************/
typedef struct spla0100 {
     int  bytes_return;
     int  bytes_avail;
     char int_job_IDы16Е;
     char int_splf_IDы16Е;
     char job_nameы10Е;
     char usr_nameы10Е;
     char job_numberы6Е;
     char splf_nameы10Е;
     int  splf_number;
     char form_typeы10Е;
     char usr_dataы10Е;
     char statusы10Е;
     char file_availы10Е;
     char hold_fileы10Е;
     char save_fileы10Е;
     int  total_pages;
     int  curr_page;
     int  start_page;
     int  end_page;
     int  last_page_print;
     int  rest_page;
     int  total_copies;
     int  copies_rem;
     int  lines_per_inch;
     int  char_per_inch;
     char output_priorityы2Е;
     char outq_nameы10Е;
     char outq_libы10Е;
     char date_file_openы7Е;
     char time_file_openы6Е;
     char dev_file_nameы10Е;
     char dev_file_libы10Е;
     char pgm_nameы10Е;
     char pgm_libы10Е;
     char count_codeы15Е;
     char print_textы30Е;
     int  record_length;
     int  max_records;
     char dev_typeы10Е;
     char ptr_dev_typeы10Е;
     char doc_nameы12Е;
     char folder_nameы64Е;
     char s36_proc_nameы8Е;
     char print_fidelы10Е;
     char repl_unprint;
     char repl_char;
     int  page_length;
     int  page_width;
     int  number_seperate;
     int  overflow_line_nm;
     char DBCS_dataы10Е;
     char DBCS_ext_charsы10Е;
     char DBCS_SOSIы10Е;
     char DBCS_char_rotateы10Е;
     int  DBCS_cpi;
     char grph_char_setы10Е;
     char code_pageы10Е;
     char form_def_nameы10Е;
     char form_def_libы10Е;
     int  source_drawer;
     char print_fontы10Е;
     char s36_spl_IDы6Е;
     int  page_rotate;
     int  justification;
     char duplexы10Е;
     char foldы10Е;
     char ctrl_charы10Е;
     char align_formsы10Е;
     char print_qualityы10Е;
     char form_feedы10Е;
     char disk_volumeы71Е;
     char disk_labelы17Е;
     char exch_typeы10Е;
     char char_codeы10Е;
     int  nmbr_disk_rcrds;
     int  multiup;
     char frnt_ovrly_nameы10Е;
     char frnt_ovrly_lib_nameы10Е;
     char frnt_ovrly_off_dnы8Е;
     char frnt_ovrly_off_acrossы8Е;
     char bck_ovrly_nameы10Е;
     char bck_ovrly_lib_nameы10Е;
     char bck_ovrly_off_dnы8Е;
     char bck_ovrly_off_acrossы8Е;
     char unit_measureы10Е;
     char page_definitionы10Е;
     char page_definition_libы10Е;
     char line_spacingы10Е;
     char point_sizeы8Е;
} spla0100;
 
 
/*******************************************************************/
/* Record structure for QUSRJOBI JOBI0100 format                   */
/*******************************************************************/
typedef struct jobi0100 {
     int  bytes_return;
     int  bytes_avail;
     char qual_job_nameы26Е;
     char int_job_IDы16Е;
     char job_statusы10Е;
     char job_type;
     char job_subtype;
     char RESERVEDы2Е;
     int  run_priority;
     int  time_slice;
     int  default_wait;
     char purgeы10Е;
} jobi0100;
 
 
/*******************************************************************/
/* Record structure for QUSRJOBI JOBI0150 format                   */
/*******************************************************************/
typedef struct jobi0150 {
     int  bytes_return;
     int  bytes_avail;
     char qual_job_nameы26Е;
     char int_job_IDы16Е;
     char job_statusы10Е;
     char job_type;
     char job_subtype;
     char RESERVEDы2Е;
     int  run_priority;
     int  time_slice;
     int  default_wait;
     char purgeы10Е;
     char time_slice_end_poolы10Е;
     int  CPU_used;
     int  system_pool_ID;
} jobi0150;
 
 
/*******************************************************************/
/* Record structure for QUSRJOBI JOBI0200 format                   */
/*******************************************************************/
typedef struct jobi0200 {
     int  bytes_return;
     int  bytes_avail;
     char qual_job_nameы26Е;
     char int_job_IDы16Е;
     char job_statusы10Е;
     char job_type;
     char job_subtype;
     char subsys_nameы10Е;
     int  run_priority;
     int  system_pool_ID;
     int  CPU_used;
     int  aux_IO_request;
     int  interact_trans;
     int  response_time;
     char function_type;
     char function_nameы10Е;
     char active_job_statы4Е;
} jobi0200;
 
 
/*******************************************************************/
/* Record structure for QUSRJOBI JOBI0300 format                   */
/*******************************************************************/
typedef struct jobi0300 {
     int  bytes_return;
     int  bytes_avail;
     char qual_job_nameы26Е;
     char int_job_IDы16Е;
     char job_statusы10Е;
     char job_type;
     char job_subtype;
     char jobq_nameы10Е;
     char jobq_libы10Е;
     char jobq_priorityы2Е;
     char outq_nameы10Е;
     char outq_libы10Е;
     char outq_priorityы2Е;
     char prn_dev_nameы10Е;
     char subm_nameы26Е;
     char subm_msgq_nameы20Е;
} jobi0300;
 
 
/*******************************************************************/
/* Record structure for QUSRJOBI JOBI0400 format                   */
/*******************************************************************/
typedef struct jobi0400 {
     int  bytes_return;
     int  bytes_avail;
     char qual_job_nameы26Е;
     char int_job_IDы16Е;
     char job_statusы10Е;
     char job_type;
     char job_subtype;
     char date_enter_sysы13Е;
     char date_job_activeы13Е;
     char accounting_codeы15Е;
     char job_desc_nameы10Е;
     char job_desc_libы10Е;
     char unit_work_IDы24Е;
     char mode_nameы8Е;
     char inq_msg_replyы10Е;
     char log_cl_pgmsы10Е;
     char brk_msg_hndlы10Е;
     char status_msg_hndlы10Е;
     char device_recov_actы13Е;
     char DDM_conv_handleы10Е;
     char date_seperate;
     int  date_format;
     char print_textы30Е;
     char subm_job_nameы26Е;
     char subm_msgq_nameы20Е;
} jobi0400;
 
 
/*******************************************************************/
/* Record structure for QUSRJOBI JOBI0500 format                   */
/*******************************************************************/
typedef struct jobi0500 {
     int  bytes_return;
     int  bytes_avail;
     char qual_job_nameы26Е;
     char int_job_IDы16Е;
     char job_statusы10Е;
     char job_type;
     char job_subtype;
     char RESERVEDы2Е;
     int  end_severity;
     int  log_severity;
     char logging_lvl;
     char logging_typeы10Е;
} jobi0500;
 
 
/*******************************************************************/
/* Record structure for QUSRJOBI JOBI0600 format                   */
/*******************************************************************/
typedef struct jobi0600 {
     int  bytes_return;
     int  bytes_avail;
     char qual_job_nameы26Е;
     char int_job_IDы16Е;
     char job_statusы10Е;
     char job_type;
     char job_subtype;
     char job_switchы8Е;
     char end_status;
     char subsys_descы20Е;
     char curr_usrprf_nameы10Е;
} jobi0600;
 
 
/*******************************************************************/
/* Record structure for QUSRJOBI JOBI0700 format                   */
/*******************************************************************/
typedef struct jobi0700 {
     int  bytes_return;
     int  bytes_avail;
     char qual_job_nameы26Е;
     char int_job_IDы16Е;
     char job_statusы10Е;
     char job_type;
     char job_subtype;
     char RESERVEDы2Е;
     int  lib_in_syslibl;
     int  prod_libs;
     int  curr_libs;
     int  libs_in_usrlibl;
     char libs_in_liblы43Еы11Е;
} jobi0700;
 
#endif  /* _TMKHOPNA_H */
 
