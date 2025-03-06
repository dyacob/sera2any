/*
**  sera2any v0.25
**
**  Generalized code to convert SERA (The System for Ethiopic Representation
**  in ASCII) to an arbitrary second system.  The generalized transcription
**  is achieved here by requiring a mapping of the output system into the
**  "fidel" address table (see fidel.h).
** 
**  This is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 1, or (at your option)
**  any later version.  See the included "COPYING" file.
** 
**  --Daniel Yaqob, 1995
** 
*/

#define SERA_UTIL "sera2any"
#define SERA_UTIL_MAJOR_VERSION 0.2
#define SERA_UTIL_MINOR_VERSION 5

#include <signal.h>
#include <stdio.h>
#include <syslog.h>
#include "fidel.h"
#include "rtf.map"
#include "libeth.h"

/* #define DEFLANG  amh */
/* #define DEFLANG  gez */
#define DEFLANG  tir 

extern char *TeXFidel[];
extern int UniToTraditional[];
extern LS LnS[];

typedef struct {
         boolean yes, html;
         enum IOput  out;
       } STATSFlags;

static struct mystats {
         int  stat;
         unsigned char*  name;
       } Stats[UNITOTAL];

#ifdef __STDC__
int flag_reset ( int argc, char** argv, FILE** fp, STATSFlags** statsFlags, SERAFlags** seraFlags );
int gt ( struct mystats** A, struct mystats** B );
int get_traditional ( int trad, FCHAR* uni );
void print_stats ( STATSFlags* statsFlags, SERAFlags* seraFlags ); 
int fout_java ( FCHAR* fstring, FILE* fp, SERAFlags* seraFlags );
int fout_rtf ( FILE* fp, STATSFlags* statsFlags, SERAFlags* seraFlags );
static void sig_term ( int signo );
# ifdef  SUPPORT_TEX
int fout_tex  ( FCHAR* fstring, FILE* fp, SERAFlags* seraFlags );
# endif  /* SUPPORT_TEX */
#else
int flag_reset (),
int gt ();
int get_traditional ();
void print_stats ();
int fout_java ();
int fout_rtf ();
static void sig_term ();
# ifdef  SUPPORT_TEX
int fout_tex  ();
# endif  /* SUPPORT_TEX */
#endif

/*-------------------------------------------------------------------------//
//                                                                         //
//  Main reads input switches, does file read-in, and sends output where   //
//  we want it.                                                            //
//                                                                         //
//-------------------------------------------------------------------------*/

void main (argc,argv)
  int argc;
  char **argv;
{

FCHAR* uniString;
FILE *inptr = stdin;
FILE* outptr = stdout;
SERAFlags* seraFlags;
STATSFlags* statsFlags;
int i, test = true;


  if (signal(SIGTERM, sig_term) == SIG_ERR)    /* catch kill commands */
    fprintf (stderr, "can't catch SIGTERM");
  if (signal(SIGINT, sig_term) == SIG_ERR)     /* catch Control-C     */
    fprintf (stderr, "can't catch SIGINT");

  flag_reset (argc, argv, &inptr, &statsFlags, &seraFlags);

  if ( seraFlags->rtf )
    fout_rtf ( inptr, statsFlags, seraFlags );

  while (!feof(inptr) && test)
   {
     test = fidel_fgets (&uniString, WSIZE, inptr, seraFlags);
     switch ( seraFlags->out )
	   {

       /*  TeX and Java are not character codes schemes so we
	    *  handle I/O at this level and not in the library.
		*/

         case java:  
		   fout_java (uniString, outptr, seraFlags);
		   break;
#ifdef  SUPPORT_TEX
         case tex:  
         case latex:
		   fout_tex (uniString, outptr, seraFlags);
		   break;
#endif  /* SUPPORT_TEX */

         default:
		   fidel_fputs (uniString, outptr, seraFlags);
	   }
     if (statsFlags->yes)
       {
         i = 0;
         while ( uniString[i] )
           {
             if ( isethio (uniString[i]) )
               if ( isprivate(uniString[i]) )
                 Stats[(UNITOTAL-1) + (uniString[i] - PRIVATE_USE_END)].stat ++;
               else
                 Stats[uniString[i]-UNIFIDEL].stat ++;
             i++;
           }
       }
     free (uniString);
   }

  if (statsFlags->yes) 
    print_stats ( statsFlags, seraFlags );

/*-------Lets Be A Good Citizen-----------------*/

  fclose (inptr);
  free (seraFlags);
  free (statsFlags);
  exit (0);

}



/*-------------------------------------------------------------------------//
//                                                                         //
//  Print Table of Character Occurances                                    //
//                                                                         //
//-------------------------------------------------------------------------*/



void 
print_stats (statsFlags, seraFlags)
  STATSFlags* statsFlags;
  SERAFlags* seraFlags;
{

int trad, uniMap, charBase, i, mod, space=0, stop;
FCHAR uni=0, *uniP;
FILE* fp;
struct mystats* PStats[UNITOTAL];


  uniP =& uni;
  seraFlags->out = statsFlags->out;


  fp = fopen ( "fidel.stats", "w" );
  if ( statsFlags->html ) 
    fprintf (fp, "<p><hr><p>\n<pre>\n");
  else
    fprintf (fp, "-------------------------------------------------------------------------------------\n\n");

  for (trad = 0; trad < UNITOTAL; trad += ROW) 
    {
      charBase = get_traditional ( trad, uniP );

      mod = get_fmodulo ( uni ); 

      if ( uni == SPACE || uni == MYA || uni == ONE )
        if ( statsFlags->html ) 
          fprintf (fp, "<p><hr><p>\n");
        else
          fprintf (fp, "-------------------------------------------------------------------------------------\n\n");

      if ( uni == SPACE ) 
        space = 1;

      for (i=0; i<mod; i++) 
        {
          if ( (mod == 12 && i > 6) || (mod == 15) )
            uniMap = get_traditional (trad+i, uniP);
          else
            uniMap = charBase + i;
          fidel_sputc (uni, &Stats[uniMap].name, seraFlags);
          fprintf (fp, "%*s    ", 3+space, Stats[uniMap].name);
          uni++;
        }
      fprintf (fp, "\n");
      if ( mod == 12 )                           /* start over, uniMap was trashed */
        uniMap = get_traditional ( trad, uniP );
      for (i=0; i<mod; i++) 
        {
          if ( mod == 12 && i > 6 )
            uniMap = get_traditional (trad+i, uniP);
          else
            uniMap = charBase + i;
          fprintf (fp, "%*d    ", 3+space, Stats[uniMap].stat);
        }
      fprintf (fp, "\n\n");
      if (mod > 8)
        {
          if (mod == 12)
            trad += ROW;
          else if ( (uni-mod) == ONE )
            trad += 2;
          else if ( (uni-mod) == (TEN+1) )   /* (TEN+1) = TWENTY */
            trad += 2;
          else if ( isfpunct(uni) )
            trad = UNITOTAL;
        }

    }

  if ( statsFlags->html ) 
    fprintf (fp, "</pre>\n<p><hr><p>\n");
  else
    fprintf (fp, "-------------------------------------------------------------------------------------");
  fclose(fp);
 
  for ( i = 0; i < UNITOTAL; i++ )
    PStats[i] =& Stats[i]; 

  fp = fopen("fidel2.stats", "w");
  qsort ( PStats, UNITOTAL, sizeof(struct mystruct *), gt );
  space = stop = 0;

  if ( statsFlags->html ) 
    fprintf (fp, "<p><hr><p>\n<pre>");

  for (trad = 0; trad < UNITOTAL; trad += ROW) 
    {
      fprintf (fp, "\n");
      for (i=0; i < 8; i++) 
        fprintf (fp, "%0*d    ", 3+space, trad+i);
      fprintf (fp, "\n");
      stop = 0;

      for (i=0; stop != 8 && (trad+i) < UNITOTAL; i++) 
        if ( PStats[trad+i]->name != '\0' )
          {
            fprintf (fp, "%*s    ", 3+space, PStats[trad+i]->name);
            stop++;
          }
      fprintf (fp, "\n");
      stop = 0;

      for (i=0; stop != 8 && (trad+i) < UNITOTAL; i++) 
        if ( PStats[trad+i]->name != '\0' )
          {
            fprintf (fp, "%*d    ", 3+space, PStats[trad+i]->stat);
            stop++;
          }
      fprintf (fp, "\n\n");
      if (i > ROW)
        trad += (i-ROW);

    }
  if ( statsFlags->html ) 
    fprintf (fp, "</pre>\n<p><hr><p>\n");

  fclose(fp);
  return;

}


 /*****************************************************************************/
/*****************************************************************************/
/* get_traditional
/*
/* is here becuase fidel.map does not have a TraditionalToUni
/* ...maybe it should ?
/*
/******************************************************************************/
/*****************************************************************************/

int 
get_traditional ( trad, uni )
  int trad; 
  FCHAR* uni;
{

int uniMap;


  uniMap = *uni = 0;
  while ( UniToTraditional[uniMap] != trad && uniMap < UNITOTAL ) 
    (uniMap)++;

  if ( uniMap == UNITOTAL ) /* nothing found */
    return ( NIL );

  *uni = uniMap + UNIFIDEL;

  if ( *uni >= UNILAST+NUM_EXTEND )
    *uni = (*uni - (UNILAST + NUM_EXTEND) ) + PRIVATE_USE_END - NUM_SPECIAL;

  return ( uniMap ) ;

}


int 
gt ( A, B )
  struct mystats **A, **B;
{
  return ( (*B)->stat - (*A)->stat );
}


/*-------------------------------------------------------------------------//
//                                                                         //
//  Simple routine to read input options and reset the default flags.      //
//  The flags for this version are -l if a file starts in Latin and        //
//  -s to use Ethiopic word separators in Ethiopic text zones.             //
//                                                                         //
//-------------------------------------------------------------------------*/


int 
flag_reset (argc, argv, fp, statsFlags, seraFlags)
  int argc;
  char **argv;
  FILE** fp;
  STATSFlags** statsFlags;
  SERAFlags** seraFlags;
{

LS* default_lang =& LnS[DEFLANG];
char* Lnames2[NUMLANGS] = LNAMES2;
char* Lnames3[NUMLANGS] = LNAMES3;
register int i,j;


/*---------------- Define and Initialize Flag Values -------------------------*/

  *seraFlags                =  (SERAFlags *) malloc ( sizeof(SERAFlags) );
  (*seraFlags)->top         =  default_lang; 
  (*seraFlags)->minor       =  (*seraFlags)->top;
  (*seraFlags)->major       =& LnS[lat];
  (*seraFlags)->lastchar    =  '\0';
  (*seraFlags)->dos         =  0;
  (*seraFlags)->rtf         =  false;
  (*seraFlags)->html        =  false;
  (*seraFlags)->qmark       =  false;
  (*seraFlags)->colon       =  false;
  (*seraFlags)->gspace      =  false;
  (*seraFlags)->verbatim    =  false;
  (*seraFlags)->lastpunct   =  false;
  (*seraFlags)->in          =  jun;
  (*seraFlags)->out         =  jun;
  (*seraFlags)->other_state =  false;

  *statsFlags               =  (STATSFlags *) malloc ( sizeof(STATSFlags) );
  (*statsFlags)->yes        =  false;
  (*statsFlags)->html       =  false;
  (*statsFlags)->out        =  sera;


  i = 0;

/* Who Am I? */

  if ( strncmp( &argv[strlen(argv[0])-8], "sera2any", 8) )
    if ( strstr(argv[0], "sera2jis") ) 
      (*seraFlags)->out  =  jis;
    else if ( strstr(argv[0], "sera2jun") || strstr(argv[0], "sera2ps") ) 
      (*seraFlags)->out  =  jun;
    else if ( strstr(argv[0], "sera2java") ) 
      (*seraFlags)->out  =  java;
    else if ( strstr(argv[0], "sera2uni") ) 
      (*seraFlags)->out  =  uni;
    else if ( strstr(argv[0], "sera2agafari") ) 
      (*seraFlags)->out  =  agafari;
    else if ( strstr(argv[0], "sera2feedel") ) 
      (*seraFlags)->out  =  feedel;
    else if ( strstr(argv[0], "sera2alxet") ) 
      (*seraFlags)->out  =  alxet;
    else if ( strstr(argv[0], "sera2washra") ) 
      (*seraFlags)->out  =  washra;
    else if ( strstr(argv[0], "sera2tex") ) 
#ifdef SUPPORT_TEX
      (*seraFlags)->out  =  tex;
#else
      {
        fprintf (stderr, "\a TeX Output Not Available By This Package \n");
        exit(0);
      }
#endif
    


/* What Do You Want From Me? */

  while ( ++i < argc )
    {
      if ( argv[i][0] == '-' ) 
        {
          switch ( toupper(argv[i][1]) ) 
            {
              case 'C' :  (*seraFlags)->colon = true;
                          break;
              case 'D' :  (*seraFlags)->out = debug;
                          break;

              case 'F' :  
#ifdef SUPPORT_DOS
                          (*seraFlags)->dos =  1;
#else
                            {
                              fprintf (stderr, "\a -fromdos option not available\n");
                              fprintf (stderr, "\a See ``README.dosio'' to enable DOS support\n");
                              exit(0);
                            }
#endif
                          break;

              case 'H' :  if ( !strcmp ( &argv[i][1], "html") )
                            (*seraFlags)->html  =  true;
                          else
                            goto USAGE;
                          break;

              case 'L' :  j=-1;
                          while( ++j < NUMLANGS && (strcmp(argv[i+1],Lnames2[j]) && strcmp(argv[i+1],Lnames3[j])) );
                          if (j < NUMLANGS ) 
                            (*seraFlags)->minor =& LnS[j];  
                          (*seraFlags)->top     =& LnS[j];  
                          if (j == lat)
                            (*seraFlags)->major = default_lang;
                          i++;
                          if (j == NUMLANGS) 
                            {
                              fprintf (stderr, "Language %s Not Supported\n",argv[i]);
                              exit(1);
                            }
                          break; 

              case 'O' :  i++;
                          if ( strstr ( argv[i], "-html" ) )
                            (*seraFlags)->html = true;
                          if ( strstr ( argv[i], "-rtf" ) )
                            (*seraFlags)->rtf = true;
                          if ( !strncmp (argv[i], "jun", 3) )
                            (*seraFlags)->out  =  jun;
                          else if ( !strncmp (argv[i], "jis", 3) )
                            (*seraFlags)->out  =  jis;
                          else if ( !strncmp (argv[i], "jav", 3) )
                            (*seraFlags)->out  =  java;
                          else if ( !strncmp (argv[i], "uni", 3) )
                            (*seraFlags)->out  =  uni;
                          else if ( !strncmp (argv[i], "agafari", 7) )
                            (*seraFlags)->out  =  agafari;
                          else if ( !strncmp (argv[i], "feedel", 6) )
                            (*seraFlags)->out  =  feedel;
                          else if ( !strncmp (argv[i], "alxet", 5) )
                            (*seraFlags)->out  =  alxet;
                          else if ( !strncmp (argv[i], "washra", 6) )
                            (*seraFlags)->out  =  washra;
                          else if ( !strcmp (argv[i], "debug") )
                            (*seraFlags)->out = debug;
                          else if ( !strcmp (argv[i], "tex") )
#ifdef SUPPORT_TEX
                            (*seraFlags)->out  =  tex;
#else
                            {
                              fprintf (stderr, "\a TeX Output Not Available By This Package \n");
                              exit(0);
                            }
#endif

                          else if ( !strcmp (argv[i], "latex") )
#ifdef SUPPORT_TEX
                            (*seraFlags)->out  =  latex;
#else
                            {
                              fprintf (stderr, "\a LaTeX Output Not Available By This Package \n");
                              exit(0);
                            }
#endif
                          else if ( !strncmp (argv[i], "sera") )
                            (*seraFlags)->out  =  sera;

                          break;

              case 'Q' :  (*seraFlags)->qmark = true;
                          break;
              case 'R' :  (*seraFlags)->rtf = true;
                          break;
              case 'S' :  if ( toupper(argv[i][2]) == 'T' )
                            {
                              (*statsFlags)->yes = true;            /* give character occurance statistics */
                              for ( j = 0; j < UNITOTAL; j++ )
                                Stats[j].stat = 0;
                              if ( argv[i+1] == '\0' || argv[i+1][0] == '-' )
                                break;
                              i++;

                              /*  hmmm... we need to work on making tex a
                               *  stats output option...
                               */
                              if ( strstr ( argv[i], "-html" ) )
                                (*statsFlags)->html = true;
                              if ( !strncmp (argv[i], "jun", 3) )
                                (*statsFlags)->out  =  jun;
                              else if ( !strncmp (argv[i], "jis", 3) )
                                (*statsFlags)->out  =  jis;
                              else if ( !strncmp (argv[i], "uni", 3) )
                                (*statsFlags)->out  =  uni;
                              else if ( !strncmp (argv[i], "jav", 3) )
                                (*statsFlags)->out  =  java;
                              else if ( !strncmp (argv[i], "sera", 4) )
                                (*statsFlags)->out  = sera;
                              else if ( !strncmp (argv[i], "agafari", 7) )
                                (*statsFlags)->out  =  agafari;
                              else if ( !strncmp (argv[i], "feedel", 6) )
                                (*statsFlags)->out  =  feedel;
                              else if ( !strncmp (argv[i], "alxet", 5) )
                                (*statsFlags)->out  =  alxet;
                              else if ( !strncmp (argv[i], "washra", 6) )
                                (*statsFlags)->out  =  washra;
                              else
                                i--;  /* no alternative option given, stick with default */
                            }
                          else
                            (*seraFlags)->gspace = true;  /* use Eth Word Sep. for " " */
                          break;

              case 'T' :  
#ifdef SUPPORT_DOS
                          (*seraFlags)->dos =  2;
#else
                            {
                              fprintf (stderr, "\a -todos option not available\n");
                              fprintf (stderr, "\a Recompile sera2any and libeth with ``make withdos'' \n");
                              exit(0);
                            }
#endif
                          break;

              case 'V' :  fprintf (stdout, "This is %s Version %0.1f%i\n", SERA_UTIL, SERA_UTIL_MAJOR_VERSION, SERA_UTIL_MINOR_VERSION);
                          fprintf (stdout, "  Export Date:  Wed Oct  2 10:32:00 EET 1996\n");
                          exit(1);
                          break;
USAGE:
              case '?' :
              default  :
                          fprintf (stderr, "\n\tUseage:  sera2any option[s] file\n");
                          fprintf (stderr, "\tTo substitute Latin spaces with Ge'ez wordspace:\n");
                          fprintf (stderr, "\t   -s\n");
                          fprintf (stderr, "\tTo substitute Latin colon with Ge'ez colon:\n");
                          fprintf (stderr, "\t(instead of wordspace):\n");
                          fprintf (stderr, "\t   -c\n");
                          fprintf (stderr, "\tTo switch the mappings for ?  and `? :\n");
                          fprintf (stderr, "\t   -q\n");
                          fprintf (stderr, "\tTo print tables of statistics in fidel.out and fidel2.out\n");
                          fprintf (stderr, "\t   -stats [encoding]\n");
                          fprintf (stderr, "\t           Encoding is one of: jun, jis, uni (default is sera)\n");
                          fprintf (stderr, "\tTo specify output encoding:\n");
                          fprintf (stderr, "\t   -o encoding-name \n");
                          fprintf (stderr, "\t      agafari = Agafari ANSI Mapping \n");
                          fprintf (stderr, "\t      alxet = ALXEthiopian ANSI Mapping \n");
                          fprintf (stderr, "\t      feedel = Feedel ANSI Mapping \n");
                          fprintf (stderr, "\t      java = Java \n");
                          fprintf (stderr, "\t      jis  = JIS \n");
                          fprintf (stderr, "\t      jun  = JUNET \n");
#ifdef SUPPORT_TEX
                          fprintf (stderr, "\t      latex = LaTeX \n");
                          fprintf (stderr, "\t      tex = TeX \n");
#endif
                          fprintf (stderr, "\t      uni = Unicode \n");
                          fprintf (stderr, "\t      washra = Washra (Ethio ASCII)\n");
                          fprintf (stderr, "\tTo set starting language:\n");
                          fprintf (stderr, "\t   -l iso639-name \n");
                          fprintf (stderr, "\t      am = amh = Amharic \n");
                          fprintf (stderr, "\t      gz = gez = Ge'ez   \n");
                          fprintf (stderr, "\t      la = lat = Latin   \n");
                          fprintf (stderr, "\t      ti = tir = Tigrigna\n");
                          fprintf (stderr, "\tTo force HTML conversion:\n");
                          fprintf (stderr, "\t   -html\n");
                          fprintf (stderr, "\t   or append -html to encoding name as in:\n");
                          fprintf (stderr, "\t      -o jis-html\n");
#ifdef SUPPORT_DOS
                          fprintf (stderr, "\tTo strip out DOS ^M end of line characters from input:\n");
                          fprintf (stderr, "\t   -fromdos\n");
                          fprintf (stderr, "\tTo insert DOS ^M end of line characters in output:\n");
                          fprintf (stderr, "\t   -todos\n");
#endif
                          fprintf (stderr, "\tEcho version number and quit:\n");
                          fprintf (stderr, "\t   -v\n");
                          fprintf (stderr, "\n\tTo contact the maintainers:\n");
                          fprintf (stderr, "\t   fisseha@cig.mot.com / yacob@padis.gn.apc.org\n\n");

                          exit(1);
                          break;
             
               }
           }  
         else 
           {
             fclose (*fp);
             if ((*fp = fopen(argv[i], "r")) == NULL)
               {
                 fprintf (stderr, "\n*** File Not Found \"%s\" ***\n",argv[i]);
                 exit(1);
               }
           }
  }

  return(true);

    /* else ignore, and use defaults */
}


/*
 *  Simple routine to prefix Java escape sequences to post-ANSI chars
 *  and then write them to the passed output stream.
 *    This may or may not be useful...
 */
int
fout_java (fstring, fp, seraFlags)
  FCHAR* fstring;
  FILE* fp;
  SERAFlags* seraFlags;
{

  int i=-1, count=0;

  if ( fp == NULL || feof(fp) || fstring == NULL || seraFlags == NULL )
    return ( _S_FAIL );
  
  while ( fstring[++i] )
    if ( fstring[i] < ANSI )
      fprintf (fp, "%c", fstring[i]);
    else
      {
        count++;
        fprintf (fp, "\\u%4i", fstring[i]);
      }

  return ( count );

}

#ifndef CHARMAP
typedef struct {unsigned char first; unsigned char next; } CHARMAP;
#endif /* CHARMAP */

extern CHARMAP AgafariFidel[], ALXEtFidel[], FeedelFidel[], WashraFidel[];

/*
 *  Simple routine to prefix Java escape sequences to post-ANSI chars
 *  and then write them to the passed output stream.
 */
int
fout_rtf (fp, statsFlags, seraFlags)
  FILE* fp;
  STATSFlags* statsFlags;
  SERAFlags* seraFlags;
{
#define ROMAN "Times New Roman"
#define FONTSIZE  "\\fs20 "        /* hackers can change this, 20 = 10, RTF seems to use a "divide by 2" system.. */
                                   /* DO NOT DELETE SPACE, " ", AT THE END!! */

int i, j, test = true;
FCHAR *uniString, mapAddr;
CHARMAP* myCharmap;
unsigned char *coco, rtfAddress;
char* FidelFonts[] = {            /* at the moment I don't want to relate these to the enum-ed values */
        " AGF - Zemen",
        " ALXethiopian",
        "\\fcharset2 Geezigna",
        "\\fcharset2 Washra  Primary",
        "\\fcharset2 Washrax Secondary"
};
char defRoman[80], defFidel[80], defFidel2[80], printString[80];
enum Fonts {Fagafari, Falxet, Ffeedel, Froman, Fwashra, Fwashrax} fidelFont, lastFont;



  /*
   *  Write Obligatory RTF Header      maybe we need \f instead of \r ?
   *  This is based on saving sample text with the Win '95 Word Pad.
   *  Any of this changes later. The fidel font is now fixed as "f4".
   */
  fprintf (stdout, "{\\rtf1\\ansi\\deff0\\deftab720{\\fonttbl{\\f0\\fnil MS Sans Serif;}{\\f1\\fnil\\fcharset2 Symbol;}{\\f2\\fswiss\\fprq2 System;}{\\f3\\fnil %s;}", ROMAN );

  switch (seraFlags->out)  /* do some font/coding specific tasks here */
    {
      case agafari :
        myCharmap = AgafariFidel;
		fidelFont = Fagafari;
        fprintf ( stdout, "{\\f4\\fnil\\fprq2%s;}}\r\n", FidelFonts[0] );
        break;

      case alxet :
        myCharmap = ALXEtFidel;
		fidelFont = Falxet;
        fprintf ( stdout, "{\\f4\\fnil\\fprq2%s;}}\r\n", FidelFonts[1] );
        break;

      case feedel :
        myCharmap = FeedelFidel;
		fidelFont = Ffeedel;
        fprintf ( stdout, "{\\f4\\fnil\\fprq2%s;}}\r\n", FidelFonts[2] );
        break;

      case washra :
/*      myCharmap = WashraFidel; */
		fidelFont = Fwashra;
        fprintf ( stdout, "{\\f4\\fnil\\fprq2%s;}", FidelFonts[3] );
        fprintf ( stdout, "{\\f5\\fnil\\fprq2%s;}}\r\n", FidelFonts[4] );
        break;

      default:
        break;
    }

  fprintf ( stdout, "{\\colortbl\\red0\\green0\\blue0;}\r\n" );
  fprintf ( stdout, "\\deflang1033\\pard" );


  /*
   *  Set up these recurring RTF strings, essentially they now work like
   *  sera language switches.
   */

  strcpy ( defRoman, "\\plain\\f3" );
  strcat ( defRoman, FONTSIZE );
  strcpy ( defFidel, "\\plain\\f4" );
  strcat ( defFidel, FONTSIZE );
  if ( seraFlags->out == washra )
    {
      strcpy ( defFidel2, "\\plain\\f5" );  
      strcat ( defFidel2, FONTSIZE );  
    }

  /*
   *  Assume text starts in Ge'ez...
   */
  lastFont = fidelFont;
  fprintf ( stdout, defFidel );


  /* 
   *   Let's Read And Write!! 
   */

  while (!feof(fp) && test)
   {
     test = fidel_fgets ( &uniString, WSIZE, fp, seraFlags );
     i = 0;
     while ( uniString[i] )
	   /*
	    *  We do this one char at a time to handle multiple ethiopic fonts simply...
		*/
       {
	     printString[0] = '\0';
         fidel_sputc ( uniString[i], &coco, seraFlags );

         j = 0;
         while ( coco[j] )
           {
             rtfAddress = coco[j];
             if ( isethio (uniString[i]) )
               {
			     mapAddr =  uniString[i];
			     mapAddr =  (isprivate(mapAddr))  
                         ?  (UNITOTAL-1)  + (mapAddr - PRIVATE_USE_END)
                         :  (mapAddr - UNIFIDEL)
                         ;
                 if (seraFlags->out == washra)
                   {
                     if ( WashraFidel[mapAddr].first == '\0' )
                       fidelFont = Fwashrax;
                     else
                       fidelFont = Fwashra;
                   }
                 if (lastFont != fidelFont)
                   {
                     if ( fidelFont == Fwashrax )
                       strcat ( printString, defFidel2 );
                     else
                       strcat ( printString, defFidel );
                   }
                 strcat ( printString, RTFMAP[rtfAddress] );
                 lastFont = fidelFont;
               }
             else if ( coco[j] > ' ' )
               {
                 if (lastFont != Froman)
                   strcat ( printString, defRoman );
                 strcat ( printString, RTFMAP[rtfAddress] );
                 lastFont = Froman;
               }
             else  /* ASCII control char <= ' ' */
               {
                 if ( coco[j] == '\n' )             /* this does DOS->UNIX */
/*                 sprintf ( printString, "%s\r\n\\par", defRoman );  */
                   strcat ( printString, "\\\n" );  /* returns must be in roman -? */
                 else
                   strcat ( printString, RTFMAP[rtfAddress] );
/*               lastFont = Froman; */
               }

             j++;
		   }  /* end while (coco[j]) */
         free (coco);
         fprintf ( stdout, "%s", printString );
         i++;
       }  /* end while (uniString[i]) */


     if (statsFlags->yes)
       {
         i = 0;
         while ( uniString[i] )
           {
             if ( isethio (uniString[i]) )
               if ( isprivate(uniString[i]) )
                 Stats[(UNITOTAL-1) + (uniString[i] - PRIVATE_USE_END)].stat ++;
               else
                 Stats[uniString[i]-UNIFIDEL].stat ++;
             i++;
           }
       }
     free (uniString);
   }  /* end while (!feof(inptr) && test) */

  fprintf ( stdout, "\\par }\n\x00" );  /* close this file and we're done! */

  if (statsFlags->yes) 
    print_stats ( statsFlags, seraFlags );

/*-------Lets Be A Good Citizen-----------------*/

  fclose (fp);
  free (seraFlags);
  free (statsFlags);
  exit (0);
}


#ifdef  SUPPORT_TEX

/*
 *  Simple routine to write files with tex macros for Ethiopic.
 *  This escapes correspond to those in the "fidelmap.tex" file.
 *  La/TeX support is limited at this time, some formatting will
 *  suffer or need revision afterwards.
 */
int
fout_tex (fstring, fp, seraFlags)
  FCHAR* fstring;
  FILE* fp;
  SERAFlags* seraFlags;
{

  int i=-1, count=0;
  FCHAR fch;

  if ( fp == NULL || feof(fp) || fstring == NULL || seraFlags == NULL )
    return ( _S_FAIL );
  
  while ( fch = fstring[++i] )
    if ( fch < ANSI )
      fprintf (fp, "%c", fstring[i]);
    else if ( fch >= UNIFIDEL && fch < UNIFIDEL+UNITOTAL )
      {
        count++;
        fch -= UNIFIDEL;
        if ( fstring[i+1] == GEMINATION )
		  {
            fprintf ( fp, "\\geminateG{\\%s}", TeXFidel[fch] );
			i++;
		  }
        else
          fprintf ( fp, "{\\%s}", TeXFidel[fch] );
      }
    else if ( fch >= PRIVATE_USE_BEGIN && fch <= PRIVATE_USE_END )
      {
        count++;
        fch  = (UNITOTAL-1)  + (fch - PRIVATE_USE_END);
        fprintf ( fp, "{\\%s}", TeXFidel[fch] );
      }

  return ( count );

}

#endif  /* SUPPORT_TEX */
 
/*-------------------------------------------------------------------------//
//                                                                         //
//  sig_term catches the more violent break signals, presumably something  //
//  has gone awry...                                                       //
//                                                                         //
//-------------------------------------------------------------------------*/

static void
sig_term ( int signo )
{

  if ( signo == SIGTERM || signo == SIGINT )
    {
      /* ...personal toying with unix...
       *
	   * openlog ("sera2", LOG_PID, LOG_USER);
	   * syslog (LOG_DEBUG, "failed on %s", filename);
	   * closelog ();
	   */ 
      fprintf (stderr, "\nIndE??  You seem to have found a bug with %s version %0.1f%i\n", SERA_UTIL, SERA_UTIL_MAJOR_VERSION, SERA_UTIL_MINOR_VERSION);
   	  fprintf (stderr, "Please email your input file to the maintainer\n");
	  fprintf (stderr, "Daniel Yaocb:-  Daniel_Yacob_at_UNECA@un.org\n");
	  fprintf (stderr, "                                              Thank You!\n\n");
	  exit (0);
    }

}
