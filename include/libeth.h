
#ifndef LIBETH_H
#define LIBETH_H  1

#include <stdio.h>

#if __G_HAVE_BOOL

typedef bool boolean;

#else

  typedef enum {false, true} boolean;

#endif  /* __G_HAVE_BOOL */

#ifndef NIL
#define NIL      -1
#endif  /* NIL */
#define WSIZE     120

#define NUMLANGS 5

enum Scripts  { fidel, latin, greek, special };
enum Languages{ amh, gez, tir, lat, gre, gfx };

enum IOput   { agafari, alxet, feedel, java, jis, jun, latex, sera, tex, uni, washra, debug };

typedef struct {
           enum Languages l; 
           enum Scripts   s;
        }  LS;
          
typedef struct {
           boolean gspace, verbatim, eth_only, html;
           boolean colon, qmark, lastpunct, rtf;
           LS   *minor, *major, *top;
           enum IOput in, out;
           int lastchar, other_state, form, dos; 
        }  SERAFlags;

#define LSDEFS {{amh,fidel},{gez,fidel},{tir,fidel},{lat,latin},{gre,greek},{gfx,special}}
#define LNAMES2 {"am", "gz", "ti", "la", "el"}
#define LNAMES3 {"amh","gez","tir","lat","gre"}


/* 
**  get macros 
*/
#define fidel_getc(fp, mysflags) fidel_fgetc(fp, mysflags)
#define fidel_getchar(myflags) fidel_fgetc(stdin, myflags)

/*
**  put macros 
*/ 
#define fidel_puts(fstring, mysflags) fidel_fputs(fstring, stdout, mysflags)
#define fidel_putc(fchar, fp, mysflags) fidel_fputc(fchar, fp, mysflags)
#define fidel_putchar(fchar, mysflags) fidel_fputc(fchar, stdout, mysflags)

#define _S_FAIL 0
#define _S_SUCCESS 1


typedef unsigned int FCHAR;

typedef struct List
  {
    int fch;
    struct List* next;
  } IntList;


typedef struct UGList
  {
    FCHAR fch;
    FILE* fp;
    struct UGList* next;
  } UnGetList;



#ifdef __STDC__

/*
**    string manipulation
*/

int fidel_strcpy ( FCHAR* target, FCHAR* fstring );

int fidel_strncpy ( FCHAR* target, FCHAR* fstring, int n );

int fidel_strcat ( FCHAR** target, FCHAR* fstring );

int fidel_strlen ( FCHAR* fstring );


/*
**    get functions 
*/


FCHAR get_fchar ( char* tstring, FILE* fp, SERAFlags* mysflags);

FCHAR sget_fchar ( char* tstring, FILE* fp, SERAFlags* mysflags );

int  sget_fstring ( FCHAR** target, char* tstring, SERAFlags* mysflags );

FCHAR fidel_fgetc ( FILE* fp, SERAFlags* mysflags );

int fidel_gets ( FCHAR* target, SERAFlags* mysflags );

int fidel_fgets ( FCHAR** target, int n, FILE* fp, SERAFlags* mysflags );

int fidel_notfscanf ( FCHAR** target, FILE* fp, SERAFlags* mysflags );

int fidel_ungetc ( FCHAR fch, FILE* fp );

FCHAR fidel_flush ( FILE* fp, SERAFlags* mysflags );

/*
**    hidden functions used by get functions, to handle ungetc
*/

FCHAR check_fidel_ungetc ( FILE* fp );

void QueInt ( IntList** q, FCHAR fch );

FCHAR PopInt ( IntList** q );

int Peep ( IntList** q );

void QueUnList ( UnGetList** q, FCHAR fch, FILE* fp );


/*
**    put functions 
*/

int fidel_sputc ( FCHAR fch, unsigned char** target, SERAFlags* mysflags );

int fidel_sputs ( FCHAR* fstring, unsigned char** target, SERAFlags* mysflags );

int fidel_fputc ( FCHAR fch, FILE* fp, SERAFlags* mysflags );

int fidel_fputs ( FCHAR* fstring, FILE* fp, SERAFlags* mysflags );

FCHAR fidel_kbhit ( SERAFlags* mysflags );


/*
** ctype functions
*/

int get_fmodulo ( FCHAR fchar );

boolean isfidel ( FCHAR fchar );

boolean isfdigit ( FCHAR fchar );

boolean isflnum ( FCHAR fchar );

boolean isethio ( FCHAR fchar );

boolean isfpunct ( FCHAR fchar );

boolean isfspace ( FCHAR fchar );

boolean isfprint ( FCHAR fchar );

boolean isfcntrl ( FCHAR fchar );

boolean isfgraph ( FCHAR fchar );

boolean isprivate ( FCHAR fchar );

int arabtof ( char* number, FCHAR** ftarget, SERAFlags* mysflags );

int ftoi ( FCHAR* fstring, SERAFlags* mysflags );


#else  /* we do not have ANSI C */

/*
**    string manipulation
*/

int fidel_strcpy (); 

int fidel_strncpy ();

int fidel_strcat ();

int fidel_strlen ();


/*
**    get functions 
*/

FCHAR get_fchar ();

FCHAR sget_fchar ();

int  sget_fstring ();

FCHAR fidel_fgetc ();

int fidel_gets ();

int fidel_fgets ();

int fidel_notfscanf ();

int fidel_ungetc ();

FCHAR fidel_flush ();

/*
**    hidden functions used by get functions, to handle ungetc
*/

FCHAR check_fidel_ungetc ();

void QueInt ();

FCHAR PopInt ();

int Peep ();

void QueUnList ();


/*
**    put functions 
*/

int fidel_sputc ();

int fidel_sputs ();

int fidel_fputc ();

int fidel_fputs ();

FCHAR fidel_kbhit();

/*
** ctype functions
*/

int get_fmodulo ( );

boolean isfidel ( );

boolean isfdigit ( );

boolean isflnum ( );

boolean isethio ( );

boolean isfpunct ( );

boolean isfspace ( );

boolean isfprint ( );

boolean isfcntrl ( );

boolean isfgraph ( );

boolean isprivate ( );

int arabtof ( );

int ftoi ( );

#endif /* ifdef __STDC__ */

#endif /* ifndef LIBETH_H */
