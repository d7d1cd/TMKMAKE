/* ================================================================= */
/*                                                                   */
/*  MODULE:    TMKEXPR                                               */
/*                                                                   */
/*                                                                   */
/*  MODULE-TYPE: C Library                                           */
/*                                                                   */
/*  Processor:  C                                                    */
/*                                                                   */
/*  Purpose:    Conditional directive expression parser              */
/*                                                                   */
/* ================================================================= */
 
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <ctype.h>
 
#include        "tmkhbase.qattsysc"
#include        "tmkhmsgh.qattsysc"
#include        "tmkhutil.qattsysc"
#include        "tmkhdict.qattsysc"
#include        "tmkhfile.qattsysc"
#include        "tmkhpars.qattsysc"
#include        "tmkhopti.qattsysc"
 
int     yyparse( Void );
Int32   yylex( Void );
 
# define TOKEN_BASE 257
# define EXIT 258
# define NUMBER 259
# define CMPL 260
# define NOT 261
# define ADD 262
# define SUB 263
# define MULT 264
# define DIV 265
# define MOD 266
# define SHFLT 267
# define SHFRT 268
# define AND 269
# define OR 270
# define EQ 271
# define NEQ 272
# define GE 273
# define GT 274
# define LE 275
# define LT 276
# define BOR 277
# define BAND 278
# define BXOR 279
# define LEFTP 280
# define RIGHTP 281
# define ENDINPUT 282
# define UMINUS 283
# define UPLUS 284
# define UCMPL 285
# define UNOT 286
#define yyclearin yychar = -1
#define yyerrok yyerrflag = 0
extern int yychar;
extern int yyerrflag;
#ifndef YYMAXDEPTH
#define YYMAXDEPTH 150
#endif
#ifndef YYSTYPE
#define YYSTYPE int
#endif
YYSTYPE yylval, yyval;
typedef int yytabelem;
# define YYERRCODE 256
 
 
/* ================================================================= */
/*  FUNCTION:    evaluate_exp                                        */
/*                                                                   */
/*  Purpose:    Evaluate the expression in the input buffer          */
/*  Input:      char *s     input buffer contains expression         */
/*              int  line   description file line #                  */
/*                                                                   */
/*  Output:     function returns                                     */
/*              = 0               expression evaluate, return value  */
/*                                stores in global variable          */
/*                                return_value.                      */
/*              = ERR_INV_SYNTAX  Invalid expression syntax          */
/*              = ERR_INV_OCT     Invalid octal number in expression */
/*              = ERR_INV_HEX     Invalid hex number in expression   */
/*              = ERR_INV_TOKEN   Invalid token in expression        */
/*              = ERR_TOO_COMPLEX Expression too complex to evaluate */
/* ================================================================= */
 
Static  Char    *bp;
Static  Int32   bc;
Static  Int16   line_no;
 
Int32   evaluate_exp ( Char *s, Int16 line )
{
        Int32   rc;
 
#ifdef  SRVOPT
        if( srvopt_function() )
            printf("FCT:evaluate_exp(\"%s\",%d)\n",s,line);
#endif
        bp              = s;
        bc              = strlen( s );
        line_no         = line;
 
        rc      = yyparse();
 
#ifdef  SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:evaluate_exp:%d\n",rc);
#endif
        return( rc );
}
 
#if     YYDEBUG
 
/* ================================================================= */
/*                                                                   */
/*  FUNCTION:    MAIN                                                */
/*                                                                   */
/*  Purpose:    Conditional directive expression parser              */
/*                                                                   */
/* ================================================================= */
 
char    bы512Е;
 
main()
{
        while( fgets( b, 511, stdin ) != NULL ) {
            switch( evaluate_exp( b, strlen( b ) ) ) {
            case -1             : exit( 0 );
            case 0              :
                printf("%d\n", return_value);
                break;
            case ERR_INV_SYNTAX :
                printf("yyparse() : ERR_INV_SYNTAX\n"); break;
            case ERR_INV_TOKEN  :
                printf("yyparse() : ERR_INV_TOKEN\n"); break;
            case ERR_INV_OCT    :
                printf("yyparse() : ERR_INV_OCT\n"); break;
            case ERR_INV_HEX    :
                printf("yyparse() : ERR_INV_HEX\n"); break;
            case ERR_TOO_COMPLEX        :
                printf("yyparse() : ERR_TOO_COMPLEX\n"); break;
            case ERR_INTERNAL_ERR:
                printf("yyparse() : ERR_INTERNAL_ERR\n"); break;
            default                     :
                printf("Error return from yyparse() = %d\n", return_value );
            }
        }
}
 
#endif  /* YYDEBUG */
 
yyerror(char *s)
{
#ifdef  SRVOPT
        if( srvopt_function() )
            printf("FCT:yyerror(\"%s\")\n",s);
#endif
        log_error( memcmp( s, "yacc stack overflow", 19 ) == 0 ?
                EXPR_TOO_COMPLICATE : EXPR_SYNTAX_ERROR, NULL, line_no );
        exit( TMK_EXIT_FAILURE );
}
 
yywrap( Void )
{
#ifdef  SRVOPT
        if( srvopt_function() )
            printf("FCT:yywrap()\n");
#endif
        log_error( INTERNAL_EXPR_ERROR, NULL, line_no );
        exit( TMK_EXIT_FAILURE );
}
 
/* ================================================================= */
/*  FUNCTION:    isodigit                                            */
/*                                                                   */
/*  Purpose:    Determine the input character is an octal digit      */
/*                                                                   */
/* ================================================================= */
 
Static
Int32   isodigit ( Int32 c )
{
        Int32   rc;
#ifdef  SRVOPT
        if( srvopt_function() )
            printf("FCT:isodigit(%c)\n",c);
#endif
        rc      = '0' <= c && c <= '7';
#ifdef  SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:isodigit:%d\n",rc);
#endif
        return( rc );
}
 
/* ================================================================= */
/*  FUNCTION:    yylex                                               */
/*                                                                   */
/*  Purpose:    Lexical analyser for the conditional directives      */
/*              expression evaluation.                               */
/*                                                                   */
/* ================================================================= */
 
Int32   yylex( Void )
{
        Int32   c;
 
#ifdef  SRVOPT
        if( srvopt_function() )
            printf("FCT:yylex()\n");
#endif
        /* skip all the leading blanks before parsing the next token */
        while( bc && *bp == ' ' ) {
            --bc; ++bp;
        }
 
        /* return ENDINPUT if no more character from the input buffer */
        if( bc == 0 )
            return ENDINPUT;
 
        c       = *bp;
        --bc; ++bp;
 
#if 0
#if     YYDEBUG
        /* special processing code during debug phase                 */
        switch( c ) {
        case 'E':       return( EXIT );         /* exit exp main pgm  */
        case 'D':       return( DEBUG );        /* toggle debug info  */
        }
#endif  /* YYDEBUG */
#endif
 
        /* parse a valid decimal/octal/hexadecimal number             */
        if( isdigit(c) ) {
            if( yylval = ( c - '0' ) ) {
                /* first digit is non zero, must be decimal */
                while( bc && isdigit( c = *bp ) ) {
                    yylval      *= 10;
                    yylval      += c - '0';
                    ++bp;       --bc;
                }
            } else {
                /* first digit is zero, check if octal or hex */
                if( bc ) {
                    c   = *bp;
                    if( c == 'x' || c == 'X' ) {
                        /* ensure the digit after 'x' */
                        /* is valid hex digits        */
                        if( isxdigit( *( bp+1 ) ) ) {
                            /* convert hex number */
                            ++bp;       --bc;
                            while( bc && isxdigit( c = *bp ) ) {
                                yylval  *= 16;
                                yylval  += isdigit(c) ? c - '0'
                                        : toupper(c) - 'A' + 10;
                                ++bp;   --bc;
                            }
                            if( bc && !isspace( *bp ) ) {
                                /* invalid hex format  */
                                log_error( INV_HEX_NO, NULL, line_no );
                                exit( TMK_EXIT_FAILURE );
                            }
                        } else {
                            /* invalid hex format  */
                            log_error( INV_HEX_NO, NULL, line_no );
                            exit( TMK_EXIT_FAILURE );
                        }
                    } else {
                        /* convert octal number */
                        while( bc && isodigit( c = *bp ) ) {
                            yylval      *= 8;
                            yylval      += c - '0';
                            ++bp;       --bc;
                        }
                        /* check for invalid octal number */
                        /* sequence                       */
                        if( bc && isdigit( c ) ) {
                            log_error( INV_OCTAL_NO, NULL, line_no );
                            exit( TMK_EXIT_FAILURE );
                        }
                    }
                }
            }
            return( NUMBER );
        }
 
        /* parse for expression operator */
        switch( c ) {
        case '\n'       :       return ENDINPUT;
        case '('        :       return LEFTP;
        case ')'        :       return RIGHTP;
        case '+'        :       return ADD;
        case '-'        :       return SUB;
        case '*'        :       return MULT;
        case '/'        :       return DIV;
        case '%'        :       return MOD;
        case '~'        :       return CMPL;
        case '??''      :       return BXOR;
        case '&'        :
            if( bc && *bp == '&' ) {
                --bc; ++bp;
                return AND;
            }
            return BAND;
        case '!'        :
            if( bc && *bp == '=' ) {
                --bc; ++bp;
                return NEQ;
            }
            return NOT;
        case '='        :
            if( bc && *bp == '=' ) {
                --bc; ++bp;
                return EQ;
            }
            break;
        case '<'        :
            if( bc && *bp == '=' ) {
                --bc; ++bp;
                return LE;
            }
            if( bc && *bp == '<' ) {
                --bc; ++bp;
                return SHFLT;
            }
            return LT;
        case '>'        :
            if( bc && *bp == '=' ) {
                --bc; ++bp;
                return GE;
            }
            if( bc && *bp == '>' ) {
                --bc; ++bp;
                return SHFRT;
            }
            return GT;
        case '??!'      :       /* vertical bar processing */
        case 0x6a       :       /* vertical bar processing */
check_more_or:
            if( bc && ( *bp == '??!' || *bp == 0x6a ) ) {
                --bc; ++bp;
                return OR;
            } else {
                    /* check for |??! condition */
                    if( bc >= 3 && *bp == '?'
                        && *(bp+1) == '?' && *(bp+2) == '!' ) {
                        bc      -= 3;
                        bp      += 3;
                        return OR;
                    }
            }
            return BOR;
        case '?'        :       /* looking for tri-graphs */
            if( bc >= 2 && *bp == '?' ) {
                int     found = 1;      /* found tr-graph flag */
 
                switch( *(bp+1) ) {
                case '!' :      bc      -= 2;
                                bp      += 2;
                                goto    check_more_or;
 
                case '=' :      c       = '??=';        break;
                case '(' :      c       = '??(';        break;
                case ')' :      c       = '??)';        break;
                case '/' :      c       = '\\';         break;
                case '\'' :     c       = BXOR;         break;
                case '<' :      c       = '??<';        break;
                case '>' :      c       = '??>';        break;
                case '-' :      c       = CMPL;         break;
                default :       found   = 0;
                }
                /* adjust bc count and pointer if tri-graph found */
                if( found ) {
                    bc  -= 2;
                    bp  += 2;
                }
            }
        }
        if( c <= TOKEN_BASE ) {
            log_error( EXPR_SYNTAX_ERROR, NULL, line_no );
            exit( TMK_EXIT_FAILURE );
        }
 
#ifdef  SRVOPT
        if( srvopt_fctrtn() )
            printf("RTN:yylex:%d\n",c );
#endif
        return( c );
}
yytabelem yyexcaыЕ ={
-1, 1,
        0, -1,
        -2, 0,
        };
# define YYNPROD 27
# define YYLAST 191
yytabelem yyactыЕ={
 
    14,    15,    11,    12,    13,    16,    17,    27,    28,    22,
    23,    21,    20,    19,    18,    25,    24,    26,     1,     0,
    10,    14,    15,    11,    12,    13,    16,    17,    27,    28,
    22,    23,    21,    20,    19,    18,    25,    24,    26,     0,
    52,    14,    15,    11,    12,    13,    16,    17,    27,     0,
    22,    23,    21,    20,    19,    18,    25,    24,    26,    14,
    15,    11,    12,    13,    16,    17,     0,     0,    22,    23,
    21,    20,    19,    18,    25,    24,    26,    14,    15,    11,
    12,    13,    16,    17,     0,     0,    22,    23,    21,    20,
    19,    18,     0,    24,    26,    14,    15,    11,    12,    13,
    16,    17,     0,     0,    22,    23,    21,    20,    19,    18,
     0,    24,    14,    15,    11,    12,    13,    16,    17,     0,
     0,    22,    23,    21,    20,    19,    18,    14,    15,    11,
    12,    13,    16,    17,    11,    12,    13,     0,    21,    20,
    19,    18,     9,     8,     4,     5,     6,     7,    14,    15,
    11,    12,    13,    16,    17,    14,    15,    11,    12,    13,
     0,     2,     0,     0,     3,    29,    30,    31,    32,    33,
     0,     0,     0,    34,    35,    36,    37,    38,    39,    40,
    41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
    51 };
yytabelem yypactыЕ={
 
  -116, -1000,  -262,  -116,  -116,  -116,  -116,  -116, -1000, -1000,
 -1000,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,
  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -116,  -241,
 -1000, -1000, -1000, -1000, -1000, -1000, -1000,  -130,  -130,  -107,
  -107,  -114,  -114,  -114,  -114,  -135,  -135,  -150,  -185,  -167,
  -203,  -221, -1000 };
yytabelem yypgoыЕ={
 
     0,    18,   161 };
yytabelem yyr1ыЕ={
 
     0,     1,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2 };
yytabelem yyr2ыЕ={
 
     0,     5,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
     7,     5,     5,     5,     5,     2,     3 };
yytabelem yychkыЕ={
 
 -1000,    -1,    -2,   280,   260,   261,   262,   263,   259,   258,
   282,   264,   265,   266,   262,   263,   267,   268,   276,   275,
   274,   273,   271,   272,   278,   277,   279,   269,   270,    -2,
    -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,
    -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,    -2,
    -2,    -2,   281 };
yytabelem yydefыЕ={
 
     0,    -2,     0,     0,     0,     0,     0,     0,    25,    26,
     1,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
    21,    22,    23,    24,     3,     4,     5,     6,     7,     8,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,     2 };
typedef struct { char *t_name; int t_val; } yytoktype;
#ifndef YYDEBUG
#       define YYDEBUG  0       /* don't allow debugging */
#endif
 
#if YYDEBUG
 
yytoktype yytoksыЕ =
{
        "TOKEN_BASE",   257,
        "EXIT", 258,
        "NUMBER",       259,
        "CMPL", 260,
        "NOT",  261,
        "ADD",  262,
        "SUB",  263,
        "MULT", 264,
        "DIV",  265,
        "MOD",  266,
        "SHFLT",        267,
        "SHFRT",        268,
        "AND",  269,
        "OR",   270,
        "EQ",   271,
        "NEQ",  272,
        "GE",   273,
        "GT",   274,
        "LE",   275,
        "LT",   276,
        "BOR",  277,
        "BAND", 278,
        "BXOR", 279,
        "LEFTP",        280,
        "RIGHTP",       281,
        "ENDINPUT",     282,
        "UMINUS",       283,
        "UPLUS",        284,
        "UCMPL",        285,
        "UNOT", 286,
        "-unknown-",    -1      /* ends search */
};
 
char * yyredsыЕ =
{
        "-no such reduction-",
        "stat : expr ENDINPUT",
        "expr : LEFTP expr RIGHTP",
        "expr : expr MULT expr",
        "expr : expr DIV expr",
        "expr : expr MOD expr",
        "expr : expr ADD expr",
        "expr : expr SUB expr",
        "expr : expr SHFLT expr",
        "expr : expr SHFRT expr",
        "expr : expr LT expr",
        "expr : expr LE expr",
        "expr : expr GT expr",
        "expr : expr GE expr",
        "expr : expr EQ expr",
        "expr : expr NEQ expr",
        "expr : expr BAND expr",
        "expr : expr BOR expr",
        "expr : expr BXOR expr",
        "expr : expr AND expr",
        "expr : expr OR expr",
        "expr : CMPL expr",
        "expr : NOT expr",
        "expr : ADD expr",
        "expr : SUB expr",
        "expr : NUMBER",
        "expr : EXIT",
};
#endif /* YYDEBUG */
/* @(#)yaccpar  1.3  com/cmd/lang/yacc,3.1,9021 9/7/89 18:46:37 */
/*
** Skeleton parser driver for yacc output
*/
 
/*
** yacc user known macros and defines
*/
#ifdef YYSPLIT
#   define YYERROR      return(-2)
#else
#   define YYERROR      goto yyerrlab
#endif
 
#define YYACCEPT        return(0)
#define YYABORT         return(1)
#define YYBACKUP( newtoken, newvalue )\
{\
        if ( yychar >= 0 || ( yyr2ы yytmp Е >> 1 ) != 1 )\
        {\
                yyerror( "syntax error - cannot backup" );\
                goto yyerrlab;\
        }\
        yychar = newtoken;\
        yystate = *yyps;\
        yylval = newvalue;\
        goto yynewstate;\
}
#define YYRECOVERING()  (!!yyerrflag)
#ifndef YYDEBUG
#       define YYDEBUG  1       /* make debugging available */
#endif
 
/*
** user known globals
*/
int yydebug;                    /* set to 1 to get debugging */
 
/*
** driver internal defines
*/
#define YYFLAG          (-1000)
 
#ifdef YYSPLIT
#   define YYSCODE { \
                        extern int (*yyfыЕ)(); \
                        register int yyret; \
                        if (yyfыyytmpЕ) \
                            if ((yyret=(*yyfыyytmpЕ)()) == -2) \
                                    goto yyerrlab; \
                                else if (yyret>=0) return(yyret); \
                   }
#endif
 
/*
** global variables used by the parser
*/
YYSTYPE yyvы YYMAXDEPTH Е;      /* value stack */
int yysы YYMAXDEPTH Е;          /* state stack */
 
YYSTYPE *yypv;                  /* top of value stack */
YYSTYPE *yypvt;                 /* top of value stack for $vars */
int *yyps;                      /* top of state stack */
 
int yystate;                    /* current state */
int yytmp;                      /* extra var (lasts between blocks) */
 
int yynerrs;                    /* number of errors */
int yyerrflag;                  /* error recovery flag */
int yychar;                     /* current input token number */
 
 
 
/*
** yyparse - return 0 if worked, 1 if syntax error not recovered from
*/
int
yyparse()
{
        /*
        ** Initialize externals - yyparse may be called more than once
        */
        yypv = &yyvы-1Е;
        yyps = &yysы-1Е;
        yystate = 0;
        yytmp = 0;
        yynerrs = 0;
        yyerrflag = 0;
        yychar = -1;
 
        goto yystack;
        {
                register YYSTYPE *yy_pv;        /* top of value stack */
                register int *yy_ps;            /* top of state stack */
                register int yy_state;          /* current state */
                register int  yy_n;             /* internal state number info */
 
                /*
                ** get globals into registers.
                ** branch to here only if YYBACKUP was called.
                */
        yynewstate:
                yy_pv = yypv;
                yy_ps = yyps;
                yy_state = yystate;
                goto yy_newstate;
 
                /*
                ** get globals into registers.
                ** either we just started, or we just finished a reduction
                */
        yystack:
                yy_pv = yypv;
                yy_ps = yyps;
                yy_state = yystate;
 
                /*
                ** top of for (;;) loop while no reductions done
                */
        yy_stack:
                /*
                ** put a state and value onto the stacks
                */
#if YYDEBUG
                /*
                ** if debugging, look up token value in list of value vs.
                ** name pairs.  0 and negative (-1) are special values.
                ** Note: linear search is used since time is not a real
                ** consideration while debugging.
                */
                if ( yydebug )
                {
                        register int yy_i;
 
                        printf( "State %d, token ", yy_state );
                        if ( yychar == 0 )
                                printf( "end-of-file\n" );
                        else if ( yychar < 0 )
                                printf( "-none-\n" );
                        else
                        {
                                for ( yy_i = 0; yytoksыyy_iЕ.t_val >= 0;
                                        yy_i++ )
                                {
                                        if ( yytoksыyy_iЕ.t_val == yychar )
                                                break;
                                }
                                printf( "%s\n", yytoksыyy_iЕ.t_name );
                        }
                }
#endif /* YYDEBUG */
                if ( ++yy_ps >= &yysы YYMAXDEPTH Е )    /* room on stack? */
                {
                        yyerror( "yacc stack overflow" );
                        YYABORT;
                }
                *yy_ps = yy_state;
                *++yy_pv = yyval;
 
                /*
                ** we have a new state - find out what to do
                */
        yy_newstate:
                if ( ( yy_n = yypactы yy_state Е ) <= YYFLAG )
                        goto yydefault;         /* simple state */
#if YYDEBUG
                /*
                ** if debugging, need to mark whether new token grabbed
                */
                yytmp = yychar < 0;
#endif
                if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
                        yychar = 0;             /* reached EOF */
#if YYDEBUG
                if ( yydebug && yytmp )
                {
                        register int yy_i;
 
                        printf( "Received token " );
                        if ( yychar == 0 )
                                printf( "end-of-file\n" );
                        else if ( yychar < 0 )
                                printf( "-none-\n" );
                        else
                        {
                                for ( yy_i = 0; yytoksыyy_iЕ.t_val >= 0;
                                        yy_i++ )
                                {
                                        if ( yytoksыyy_iЕ.t_val == yychar )
                                                break;
                                }
                                printf( "%s\n", yytoksыyy_iЕ.t_name );
                        }
                }
#endif /* YYDEBUG */
                if ( ( ( yy_n += yychar ) < 0 ) || ( yy_n >= YYLAST ) )
                        goto yydefault;
                if ( yychkы yy_n = yyactы yy_n Е Е == yychar )  /*valid shift*/
                {
                        yychar = -1;
                        yyval = yylval;
                        yy_state = yy_n;
                        if ( yyerrflag > 0 )
                                yyerrflag--;
                        goto yy_stack;
                }
 
        yydefault:
                if ( ( yy_n = yydefы yy_state Е ) == -2 )
                {
#if YYDEBUG
                        yytmp = yychar < 0;
#endif
                        if ( ( yychar < 0 ) && ( ( yychar = yylex() ) < 0 ) )
                                yychar = 0;             /* reached EOF */
#if YYDEBUG
                        if ( yydebug && yytmp )
                        {
                                register int yy_i;
 
                                printf( "Received token " );
                                if ( yychar == 0 )
                                        printf( "end-of-file\n" );
                                else if ( yychar < 0 )
                                        printf( "-none-\n" );
                                else
                                {
                                        for ( yy_i = 0;
                                                yytoksыyy_iЕ.t_val >= 0;
                                                yy_i++ )
                                        {
                                                if ( yytoksыyy_iЕ.t_val
                                                        == yychar )
                                                {
                                                        break;
                                                }
                                        }
                                        printf( "%s\n", yytoksыyy_iЕ.t_name );
                                }
                        }
#endif /* YYDEBUG */
                        /*
                        ** look through exception table
                        */
                        {
                                register int *yyxi = yyexca;
 
                                while ( ( *yyxi != -1 ) ||
                                        ( yyxiы1Е != yy_state ) )
                                {
                                        yyxi += 2;
                                }
                                while ( ( *(yyxi += 2) >= 0 ) &&
                                        ( *yyxi != yychar ) )
                                        ;
                                if ( ( yy_n = yyxiы1Е ) < 0 )
                                        YYACCEPT;
                        }
                }
 
                /*
                ** check for syntax error
                */
                if ( yy_n == 0 )        /* have an error */
                {
                        /* no worry about speed here! */
                        switch ( yyerrflag )
                        {
                        case 0:         /* new error */
                                yyerror( "syntax error" );
                                goto skip_init;
                        yyerrlab:
                                /*
                                ** get globals into registers.
                                ** we have a user generated syntax type error
                                */
                                yy_pv = yypv;
                                yy_ps = yyps;
                                yy_state = yystate;
                                yynerrs++;
                        skip_init:
                        case 1:
                        case 2:         /* incompletely recovered error */
                                        /* try again... */
                                yyerrflag = 3;
                                /*
                                ** find state where "error" is a legal
                                ** shift action
                                */
                                while ( yy_ps >= yys )
                                {
                                        yy_n = yypactы *yy_ps Е + YYERRCODE;
                                        if ( yy_n >= 0 && yy_n < YYLAST &&
                                                yychkыyyactыyy_nЕЕ == YYERRCODE)
                                        {
                                                /*
                                                ** simulate shift of "error"
                                                */
                                                yy_state = yyactы yy_n Е;
                                                goto yy_stack;
                                        }
                                        /*
                                        ** current state has no shift on
                                        ** "error", pop stack
                                        */
#if YYDEBUG
#       define _POP_ "Error recovery pops state %d, uncovers state %d\n"
                                        if ( yydebug )
                                                printf( _POP_, *yy_ps,
                                                        yy_psы-1Е );
#       undef _POP_
#endif
                                        yy_ps--;
                                        yy_pv--;
                                }
                                /*
                                ** there is no state on stack with "error" as
                                ** a valid shift.  give up.
                                */
                                YYABORT;
                        case 3:         /* no shift yet; eat a token */
#if YYDEBUG
                                /*
                                ** if debugging, look up token in list of
                                ** pairs.  0 and negative shouldn't occur,
                                ** but since timing doesn't matter when
                                ** debugging, it doesn't hurt to leave the
                                ** tests here.
                                */
                                if ( yydebug )
                                {
                                        register int yy_i;
 
                                        printf( "Error recovery discards " );
                                        if ( yychar == 0 )
                                                printf( "token end-of-file\n" );
                                        else if ( yychar < 0 )
                                                printf( "token -none-\n" );
                                        else
                                        {
                                                for ( yy_i = 0;
                                                        yytoksыyy_iЕ.t_val >= 0;
                                                        yy_i++ )
                                                {
                                                        if ( yytoksыyy_iЕ.t_val
                                                                == yychar )
                                                        {
                                                                break;
                                                        }
                                                }
                                                printf( "token %s\n",
                                                        yytoksыyy_iЕ.t_name );
                                        }
                                }
#endif /* YYDEBUG */
                                if ( yychar == 0 )      /* reached EOF. quit */
                                        YYABORT;
                                yychar = -1;
                                goto yy_newstate;
                        }
                }/* end if ( yy_n == 0 ) */
                /*
                ** reduction by production yy_n
                ** put stack tops, etc. so things right after switch
                */
#if YYDEBUG
                /*
                ** if debugging, print the string that is the user's
                ** specification of the reduction which is just about
                ** to be done.
                */
                if ( yydebug )
                        printf( "Reduce by (%d) \"%s\"\n",
                                yy_n, yyredsы yy_n Е );
#endif
                yytmp = yy_n;                   /* value to switch over */
                yypvt = yy_pv;                  /* $vars top of value stack */
                /*
                ** Look in goto table for next state
                ** Sorry about using yy_state here as temporary
                ** register variable, but why not, if it works...
                ** If yyr2ы yy_n Е doesn't have the low order bit
                ** set, then there is no action to be done for
                ** this reduction.  So, no saving & unsaving of
                ** registers done.  The only difference between the
                ** code just after the if and the body of the if is
                ** the goto yy_stack in the body.  This way the test
                ** can be made before the choice of what to do is needed.
                */
                {
                        /* length of production doubled with extra bit */
                        register int yy_len = yyr2ы yy_n Е;
 
                        if ( !( yy_len & 01 ) )
                        {
                                yy_len >>= 1;
                                yyval = ( yy_pv -= yy_len )ы1Е; /* $$ = $1 */
                                yy_state = yypgoы yy_n = yyr1ы yy_n Е Е +
                                        *( yy_ps -= yy_len ) + 1;
                                if ( yy_state >= YYLAST ||
                                        yychkы yy_state =
                                        yyactы yy_state Е Е != -yy_n )
                                {
                                        yy_state = yyactы yypgoы yy_n Е Е;
                                }
                                goto yy_stack;
                        }
                        yy_len >>= 1;
                        yyval = ( yy_pv -= yy_len )ы1Е; /* $$ = $1 */
                        yy_state = yypgoы yy_n = yyr1ы yy_n Е Е +
                                *( yy_ps -= yy_len ) + 1;
                        if ( yy_state >= YYLAST ||
                                yychkы yy_state = yyactы yy_state Е Е != -yy_n )
                        {
                                yy_state = yyactы yypgoы yy_n Е Е;
                        }
                }
                                        /* save until reenter driver code */
                yystate = yy_state;
                yyps = yy_ps;
                yypv = yy_pv;
        }
        /*
        ** code supplied by user is placed in this switch
        */
 
                switch(yytmp){
 
case 1:{        return( yypvtы-1Е );            } /*NOTREACHED*/ break;
case 2:{        yyval   = yypvtы-1Е;            } /*NOTREACHED*/ break;
case 3:{        yyval   = yypvtы-2Е * yypvtы-0Е;        } /*NOTREACHED*/ break;
case 4:{        /* keep code in separate lines to avoid */
                        /* generated code over 80 chars/line    */
                        if( yypvtы-0Е == 0 ) {
                            log_error( DIVIDED_BY_ZERO, NULL, line_no );
                            exit( TMK_EXIT_FAILURE );
                        }
                        yyval   = yypvtы-2Е / yypvtы-0Е;
                } /*NOTREACHED*/ break;
case 5:{        /* keep code in separate lines to avoid */
                        /* generated code over 80 chars/line    */
                        if( yypvtы-0Е == 0 ) {
                            log_error( DIVIDED_BY_ZERO, NULL, line_no );
                            exit( TMK_EXIT_FAILURE );
                        }
                        yyval   = yypvtы-2Е % yypvtы-0Е;
                } /*NOTREACHED*/ break;
case 6:{        yyval   = yypvtы-2Е + yypvtы-0Е;        } /*NOTREACHED*/ break;
case 7:{        yyval   = yypvtы-2Е - yypvtы-0Е;        } /*NOTREACHED*/ break;
case 8:{        /* keep code in separate lines to avoid */
                        /* generated code over 80 chars/line    */
                        yyval   = ( yypvtы-0Е != 0 )
                                ? yypvtы-2Е << yypvtы-0Е
                                : yypvtы-2Е;
                } /*NOTREACHED*/ break;
case 9:{        /* keep code in separate lines to avoid */
                        /* generated code over 80 chars/line    */
                        yyval   = ( yypvtы-0Е != 0 )
                                ? yypvtы-2Е >> yypvtы-0Е
                                : yypvtы-2Е;
                } /*NOTREACHED*/ break;
case 10:{       yyval   = yypvtы-2Е < yypvtы-0Е;        } /*NOTREACHED*/ break;
case 11:{       yyval   = yypvtы-2Е <= yypvtы-0Е;       } /*NOTREACHED*/ break;
case 12:{       yyval   = yypvtы-2Е > yypvtы-0Е;        } /*NOTREACHED*/ break;
case 13:{       yyval   = yypvtы-2Е >= yypvtы-0Е;       } /*NOTREACHED*/ break;
case 14:{       yyval   = yypvtы-2Е == yypvtы-0Е;       } /*NOTREACHED*/ break;
case 15:{       yyval   = yypvtы-2Е != yypvtы-0Е;       } /*NOTREACHED*/ break;
case 16:{       yyval   = yypvtы-2Е & yypvtы-0Е;        } /*NOTREACHED*/ break;
case 17:{       yyval   = yypvtы-2Е | yypvtы-0Е;        } /*NOTREACHED*/ break;
case 18:{       yyval   = yypvtы-2Е ??' yypvtы-0Е;      } /*NOTREACHED*/ break;
case 19:{       yyval   = yypvtы-2Е && yypvtы-0Е;       } /*NOTREACHED*/ break;
case 20:{       yyval   = yypvtы-2Е || yypvtы-0Е;       } /*NOTREACHED*/ break;
case 21:{       yyval   = ~ yypvtы-0Е;          } /*NOTREACHED*/ break;
case 22:{       yyval   = ! yypvtы-0Е;          } /*NOTREACHED*/ break;
case 23:{       yyval   = yypvtы-0Е;            } /*NOTREACHED*/ break;
case 24:{       yyval   = - yypvtы-0Е;          } /*NOTREACHED*/ break;
case 26:{       return( -1 );           } /*NOTREACHED*/ break;
}
 
 
        goto yystack;           /* reset registers in driver code */
}
 
