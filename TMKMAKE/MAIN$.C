/* ================================================================= */
/*                                                                   */
/*  MODULE:    TMKMAKEC                                              */
/*                                                                   */
/*                                                                   */
/*  MODULE-TYPE: C Library                                           */
/*                                                                   */
/*  Processor:  C                                                    */
/*                                                                   */
/*  Purpose:    STRMAKE command main() entry program                 */
/*                                                                   */
/* ================================================================= */

#include <stdio.h>
#include <stdlib.h>

#include "base"
#include "utility"
#include "file"
#include "parser"
#include "builtin"
#include "mfproc"
#include "diction"
#include "option"

int main(int argc, Char **argv)
{
  // check input options
  process_options(argc, argv);

  // initialize parser parameters
  setup_parser();

  Int32 makefile_count = no_of_makefile_mbr();
  while (makefile_count--)
  {
    // initialize dictionary symbol tables
    setup_dict();

    // initialize builtin data structures, i.e. suffixes list
    // default command & inference rules list
    setup_builtin_structures();

    // initialize parser user defined rules structures
    setup_parser_structures();

    // process all macro defined in the STRMAKE command
    setup_command_macro();

    // update macros and built-in rules defined in *LIBL/QMAKSRC(BUILTIN)
    read_qmaksrc_builtin();

    // parse input file
    parse_makefile(next_makefile_mbr(), PARSE_MAKEFILE);

    // re-process rules list for implicit builtin rules
    // i.e. use inference rules if it's needed
    apply_inference_rules();

    // process makefile by items
    process_makefile();
  }

  // get rid of any resources
  delete_usrspc();

  return (TMK_EXIT_SUCCESS);
}
