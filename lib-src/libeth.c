#include <fidel.h>
#include <libeth.h>

extern char* FidelName[];
extern int UniToMod[];

static UnGetList *ulist = NULL;
/* register int i;          this isn't permitted */


 /*****************************************************************************/
/*****************************************************************************/
/*  fidel_strcpy (target, fstring)
/*
/*  This routine is the simple analog of strcpy for the FCHAR data type.
/*  The number of charcters up to and including the first NULL (or '\0') is 
/*  copied, this number is also the return value.
/*
/*  target   -is a pointer to an FCHAR text string whom text is copied to. 
/*  fstring  -is a pointer to an FCHAR text string from whom text is copied. 
/*
/******************************************************************************/
/*****************************************************************************/


int
fidel_strcpy (target, fstring) 
  FCHAR* target;
  FCHAR* fstring;
{

register int i;


  if ( fstring == NULL )
    return (_S_FAIL);

  for ( i = 0; fstring[i-1] != '\0'; i++ )
     target[i] =  fstring[i];

  return ( i );

}



 /*****************************************************************************/
/*****************************************************************************/
/*  fidel_strncpy (target, fstring, n)
/*
/*  This routine is the simple analog of strncpy for the FCHAR data type.
/*  ``n'' is also the return value.
/*
/*  target   -is a pointer to an FCHAR text string whom text is copied to. 
/*  fstring  -is a pointer to an FCHAR text string from whom text is copied. 
/*  n        -is the number of fchars to copy.
/*
/******************************************************************************/
/*****************************************************************************/


int
fidel_strncpy (target, fstring, n)
  FCHAR* target;
  FCHAR* fstring;
  int n;
{

register int i;

  
  if ( fstring == NULL )
    return (_S_FAIL);

  for ( i = 0; i <= n; i++ )
     target[i] =  fstring[i];

  return ( n );

}



 /*****************************************************************************/
/*****************************************************************************/
/*  fidel_strcat (target, fstring)
/*
/*  This routine is the simple analog of strncat for the FCHAR data type.
/*  The new string length of the target string is the return value.
/*
/*  target   -is the address of a pointer to an FCHAR text string whom text is 
/*            appended onto. 
/*  fstring  -is a pointer to an FCHAR text string from whom text is copied. 
/*
/******************************************************************************/
/*****************************************************************************/


int
fidel_strcat (target, fstring)
  FCHAR** target;
  FCHAR* fstring;
{

register int i;
int m,n,z;
FCHAR *temp, *temp2;


  if ( fstring == NULL )
    return (_S_FAIL);

  m = n = 0;
  m = fidel_strlen ( *target ) ;
  n = fidel_strlen ( fstring ) ;
  fprintf (stdout,"$%d,%d$\n",m,n);
  
  z += ( *target == NULL ) ? m + n : m + n - 1; 

  temp = (FCHAR *) malloc ( z * sizeof (FCHAR) );

  for ( i = 0; i < m-1; i++ )       /* do not copy \0 */
   {
     temp[i] =  (*target)[i];
     fprintf (stdout,"(%d)",temp[i]);
   }

  for ( i = 0; i < n; i++ )         /* copy \0 */
   {
     temp[(m-1)+i] =  fstring[i];
     fprintf (stdout,"[%d]",temp[(m-1)+i]);
   }

  temp2   = *target;
  *target = temp;
  free (temp2);

  return ( m + n - 1 );

}




 /*****************************************************************************/
/*****************************************************************************/
/*  fidel_strlen (fstring)
/*
/*  This routine is the simple analog of strlen for the FCHAR data type.
/*  The number of charcters up to and including the first NULL (or '\0') is 
/*  returned.
/*
/*  fstring  -is an FCHAR pointer. 
/*
/******************************************************************************/
/*****************************************************************************/


int
fidel_strlen (fstring)
  FCHAR* fstring;
{

register int i;
 

  if ( fstring == NULL )
    return ( 0 );

  i = 0 ;
  while ( fstring[i] ) 
    i++ ;

/*return ( i+1 ) ; */
  return ( i ) ;   

}



 /*****************************************************************************/
/*****************************************************************************/
/*  fidel_fgetc (fp, mysflags)
/*
/*  This routine is the simple analog of fgetc for the FCHAR data type.
/*  Return address of next token from a file of SERA text.
/*  The address of the returned token is specified by the character coding
/*  system passed in myflags->out.
/*
/*  fp       -is a file pointer to SERA text
/*  mysflags -is a SERA Flag structure which MUST contain a valid input and
/*            output type.
/*
/******************************************************************************/
/*****************************************************************************/


FCHAR
fidel_fgetc (fp, mysflags)
  FILE* fp;
  SERAFlags* mysflags;
{

FCHAR start;


  if ( fp == NULL || feof(fp) || mysflags == NULL)             /* bail if pointing nowhere */
    return ( _S_FAIL );

  start = check_fidel_ungetc ( fp );              /* reget ungot char, if any */

  if ( start )
    return ( start );                             /* return address if we were given one */

  return ( get_fchar(NULL, fp, mysflags) );       /* get and return next token  */

}



 /*****************************************************************************/
/*****************************************************************************/
/*  fidel_gets (target, mysflags)
/*
/*  This routine is the simple analog of gets for the FCHAR data type.
/*  This routine scans a single string from stdin.  The string length of
/*  the target string is the return value.
/*
/*  target   -is an FCHAR pointer whom we will direct to an FCHAR output 
/*            string allocated from memory by this routine.
/*  mysflags -is a SERA Flag structure which MUST contain a valid input and
/*            output type.
/*
/******************************************************************************/
/*****************************************************************************/


int
fidel_gets (target, mysflags)
  FCHAR* target;
  SERAFlags* mysflags;
{

FCHAR start;
FILE* fp;
int n;
register int i;
char text[WSIZE];
IntList* templist = NULL;


  if ( mysflags == NULL)                          /* bail if pointing nowhere */
    return ( _S_FAIL );

  fp = stdin;

  start = check_fidel_ungetc ( fp );              /* reget ungot char, if any */

  if ( start )                                    /* if token returned, add to start of list */
    {
      templist->fch = start;
      templist->next = NULL;
      start = 1;
    }

  gets ( text );                                  /* get next string from stream */
  n = strlen ( text );

  for (i = start; i < n; i++)
     QueInt ( &templist, get_fchar(NULL, fp, mysflags) );

  target = ( FCHAR * ) malloc ( n * sizeof (FCHAR) );

  for (i = 0; i < n; i++)
    target[i] = PopInt ( &templist );

  target[i] = '\0';

  return ( n );


}



 /*****************************************************************************/
/*****************************************************************************/
/*  fidel_fgets (target, n, fp, mysflags)
/*
/*  This routine is the simple analog of fgets for the FCHAR data type.
/*  This routine scans a single string of length ``n'' from the file pointed 
/*  to by ``fp''.  ``n'' is also the return value.
/*
/*  target   -is the address of an FCHAR pointer whom we will direct to an FCHAR
/*            output string allocated from memory by this routine.
/*  n        -is the number of fchars to copy.
/*  fp       -is a file pointer to SERA text
/*  mysflags -is a SERA Flag structure which MUST contain a valid input and
/*            output type.
/*
/******************************************************************************/
/*****************************************************************************/


int
fidel_fgets (target, n, fp, mysflags)
  FCHAR** target;
  int n;
  FILE* fp;
  SERAFlags* mysflags;
{

int start = 0;
register int i,j;
IntList root, *rootp;
FCHAR fch, *holdText;


  if ( ferror(fp) || feof(fp) || fp == NULL || mysflags == NULL )
    return ( _S_FAIL );

  if ( n < 1 )
    return ( _S_FAIL );

  holdText = ( FCHAR * ) malloc ( (n+1) * sizeof (FCHAR) );

  start = check_fidel_ungetc ( fp );              /* reget ungot char, if any */

  if ( start )                                    /* if token returned, add to start of list */
    {
      holdText[0] = start;
      start = 1;
    }

  for (i = start; i < n && (fch = get_fchar(NULL, fp, mysflags)) != 0; i++)
    holdText[i] = fch; 

  *target = ( FCHAR * ) malloc ( (i+1) * sizeof (FCHAR) );

  fidel_strncpy (*target, holdText, i);
  (*target)[i] = '\0';

  free (holdText);

  if (fch == EOF || fch == 0)
    return (0);


  return ( i );

}



void
QueInt ( q, fch )
  IntList** q;
  FCHAR fch;
{

IntList* temp;


  (*q)->fch = fch ;
  (*q)->next = NULL ;

  (*q)->next = ( IntList * ) malloc ( sizeof (IntList) ) ;
   *q = (*q)->next ;


  return;
}


FCHAR
PopInt ( q )
  IntList** q;
{

FCHAR fch;
IntList* p;


  if (  *q == NULL )
    return '\0' ;

  fch = (*q)->fch ;
  p = *q ;
  *q = (*q)->next;

  free ( p ); 

  return ( fch );

}


int
Peep ( q )
  IntList** q;
{
  return ( ( *q == NULL ) ? (*q)->fch : '\0' ) ;
}



 /*****************************************************************************/
/*****************************************************************************/
/*  fidel_ungetc (fch, fp)
/*
/*  This routine is the simple analog of ungetc for the FCHAR data type.
/*  The specified character ``fp'' is ungot from the file stream ``fp''.
/*  As with ANSI C the file pointer fp is NOT decremented by one char.
/*  The ungot character, fch, is stored in a linked list and associated 
/*  with the fp file pointer.  The next ungot character for stream fp then 
/*  replaces the last stored value.
/*
/*  fp       -is a file pointer to SERA text
/*
/******************************************************************************/
/*****************************************************************************/


int
fidel_ungetc ( fch, fp )
  FCHAR fch;
  FILE* fp;
{

UnGetList* startoflist;


  if ( fp == NULL || feof(fp) )                         /* bail if pointing nowhere */
    return ( _S_FAIL );

  if ( ulist == NULL )                                 /* list is empty */
    {
      QueUnList ( &ulist, fch, fp );
      return ( _S_SUCCESS );
    }

  startoflist = ulist;

  while ( ulist->fp != fp && ulist->next != NULL )     /* check list for existing definition */
    ulist = ulist->next;

  if ( ulist->fp == fp )                               /* copy over old, as per ungetc */
    {
      ulist->fch = fch;
      ulist = startoflist;
      return ( _S_SUCCESS );
    }

  QueUnList ( &ulist, fch, fp );                         /* add file and char entry to existing list */
  ulist = startoflist;
  return ( _S_SUCCESS );

}



FCHAR
check_fidel_ungetc ( fp )
  FILE* fp;
{

UnGetList *startoflist, *utemp;
FCHAR fch;


  if ( ulist == NULL )
    return ( 0 );

  startoflist = utemp = ulist;

  while ( ulist->fp != fp && ulist->next != NULL )     /* check list for existing definition */
    ulist = ulist->next;

  if ( ulist->fp == fp )                               /* copy over old, as per ungetc */
    {
      fch = ulist->fch;
      while ( utemp->next != ulist )
      utemp = utemp->next;

      utemp->next = ulist->next;                       /* deletion of node */
      free ( ulist );
      ulist = startoflist;

      return ( fch );
   }

  ulist = startoflist;
  return ( 0 );                                        /* no chars stored */
}


void
QueUnList ( q, fch, fp )
  UnGetList** q;
  FCHAR fch;
  FILE* fp;
{


  (*q)->next = ( UnGetList * ) malloc ( sizeof (UnGetList) );
  *q = (*q)->next ;

  (*q)->fch  = fch ;
  (*q)->fp   = fp;
  (*q)->next = NULL ;

  return;
}




/*
 *
 *  Following Will be Primarily The "Put" Analogs for SERA
 *
 */




 /*****************************************************************************/
/*****************************************************************************/
/*  fidel_sputs (fstring, target, mysflags)
/*
/*  Convert a string of FCHAR addresses from a supported encoding system into 
/*  another.  The string length of the converted text is returned.  
/*
/*  fstring  -is a pointer to an FCHAR text string from whom tokens are 
/*            scanned. 
/*  target   -is the address of a character pointer whom we will direct to the 
/*            output string allocated from memory by this routine.
/*  mysflags -is a SERA Flag structure which MUST contain a valid input and
/*            output type.
/*
/******************************************************************************/
/*****************************************************************************/


int
fidel_sputs (fstring, target, mysflags)
  FCHAR* fstring;
  unsigned char** target;
  SERAFlags* mysflags;
{

unsigned char *hold, *holdch;
int n,m;
register int i;


  if ( fstring == NULL || mysflags == NULL )      /* bail if pointing nowhere */
    return ( _S_FAIL );

  n = fidel_strlen (fstring); 

  hold = (char *) malloc ( (4*n+1) * sizeof(char) );

  for ( i = 0; i <= 4*n; i++ )
    hold[i] = '\0';


  for ( i = 0; i < n; i++ )
    {
      fidel_sputc (fstring[i], &holdch, mysflags); 
      strcat (hold, holdch);
      free (holdch);
    }


  *target  = ( char * ) malloc ( strlen(hold) * sizeof(char) );
  strcpy ( *target, hold );
  free (hold);

  return ( strlen(*target) );

}


int
fidel_fputc (fch, fp, mysflags)
  FCHAR fch;
  FILE* fp;
  SERAFlags* mysflags;
{

unsigned char* pch;


  if ( fp == NULL || !ferror(fp) || feof(fp) || mysflags == NULL)    /* bail if pointing nowhere */
    return ( _S_FAIL );

  if (fch == '\0' || fch == EOF)
    {
     fprintf (fp, "%c", &fch);
     return ( _S_SUCCESS );
    }

  fidel_sputs (&fch, &pch, mysflags);
  fprintf (fp, "%s", pch);

  return ( _S_SUCCESS );

}


int
fidel_fputs (fstring, fp, mysflags)
  FCHAR* fstring;
  FILE* fp;
  SERAFlags* mysflags;
{

unsigned char* pstring;


  if ( fp == NULL || feof(fp) || fstring == NULL || mysflags == NULL )
    return ( _S_FAIL );
  
  fidel_sputs (fstring, &pstring, mysflags);
  
  fprintf (fp, "%s", pstring);

  return ( _S_SUCCESS );

}


FCHAR
fidel_kbhit (mysflags)
  SERAFlags* mysflags;
{

FCHAR temp[80];
unsigned char *pch;
int count;


  if ( mysflags == NULL )
    return (_S_FAIL);

  if ( mysflags->form == 6 || mysflags->form == 0 )  /* 0 is alternate, or 0 of number */
    temp[0] = mysflags->lastchar; 

  temp[1] = getchar ();
  temp[2] = '\0';

  count = fidel_sputs (temp, &pch, mysflags) - 1;
 
  return ( (count > 1 ) ? temp[1] : temp[0] );
}





 /*****************************************************************************/
/*****************************************************************************/
/*
/*  ftoi (fstring, mysflags)
/*
/*  Convert and return an integer value from a string of Ethiopic Numbers.
/*  
/*
/*  fstring   -is a pointer to an FCHAR text string of any supported character
/*             coding system.
/*  mysflags -is a SERA Flag structure which MUST contain a valid input and
/*            output type.
/*
/*
/******************************************************************************/
/*****************************************************************************/

int 
ftoi (fstring, mysflags)
  FCHAR* fstring;
  SERAFlags* mysflags;
{

FCHAR fdig;
int i = 0, numout;


  if ( fstring == NULL || mysflags == NULL )
    return ( _S_FAIL );

  if ( fstring[0] == '-');
    i++;

  while ( (fdig = fstring[i++]) )
    if ( !isfdigit (fdig) )
      return ( _S_FAIL );

  i = numout = 0;

  if ( fstring[0] == '-');
    i++;

  while ( (fdig = fstring[i]) ) 
    {

      if ( fdig == HUNDRED || fdig == TENTHOUSAND )
        continue;

      if ( fdig >= TEN )
        fdig -= 10;

      fdig -= ( UNIFIDEL + (ONE-1) ); 
      numout += fdig * pow (10,i++);
    }

  return ( fstring[0] == '-') ? numout : -numout ;


}



 /*****************************************************************************/
/*****************************************************************************/
/*
/*  arab_tof (number, ftarget, mysflags)
/*
/*  Convert a string of ASCII digits into Ethiopic Numbers under a supported
/*  encoding system.  The string length of the converted text is returned.  
/*
/*  number   -is a pointer to an FCHAR text string of any supported character
/*            coding system.
/*  ftarget  -is the address of an FCHAR pointer whom we will direct to the 
/*            output fstring allocated from memory by this routine.
/*  mysflags -is a SERA Flag structure which MUST contain a valid input and
/*            output type.
/*
/*
/******************************************************************************/
/*****************************************************************************/


int
arabtof (number, ftarget, mysflags)
  char* number;
  FCHAR** ftarget;
  SERAFlags* mysflags;
{

register int i, j, m, n;
IntList* templist = NULL;
FCHAR fdig;
char hold[4];


  if ( number == NULL || ftarget == NULL || mysflags == NULL )
    return ( _S_FAIL );

  if ( number == NULL )
    return ( _S_FAIL );

  n = strlen ( number ) - 1;

  for (i = 0; number[i] ; i++ )  
    if ( !isdigit(number[i]) )
      return ( _S_FAIL );

  i = m = 0;

  while ( (fdig = number[n-i]) )  
    {
       
      if ( fdig == '0' )
        {
          i++;
          continue;
        }

      fdig += (ONE - '1');

      if ( i%2 )
        fdig += 9;

      QueInt ( &templist, fdig);
      m++;

      for ( j = 0; j < i/4 ; j++ )
        QueInt ( &templist, TENTHOUSAND);
      m += j;

      if ( i && i%2 )
        {
          QueInt ( &templist, HUNDRED );
          m++;
        }

      i++;
    }

  *ftarget = ( FCHAR * ) malloc ( (m+1) * sizeof (FCHAR) );

  for (i = 0; i < m; i++)
    {
      fdig = PopInt ( &templist ) - UNIFIDEL;
      strncpy ( hold, FidelName[fdig] );
      *ftarget[m-i] = get_fchar (hold, NULL, mysflags);
    }

  *ftarget[m+1] = '\0';

  return ( m );

}

 /*****************************************************************************/
/*****************************************************************************/
/*  get_fmodulo (fchar)
/*
/*  This routine determines and returns the modulo class of the passed 
/*  fidel character.
/*
/*  fchar    -a single Unicode addressed character of the FCHAR datatype
/*
/*  The number of forms of letters, and character class may be determined 
/*  by its address following:
/*
/*        0 <= address <= 175   :   8 forms
/*      176 <= address <= 319   :  12 forms
/*      320 <= address <= 368   :   7 forms
/*      369 <= address <= 382   :     extensions
/*      383 <= address <= 402   :     number range
/*      403 <= address <= 411   :     ethiopic punctuation range
/*      414 <= address <= 418   :     ethiopic-stylized punctuation range
/*      419 <= address          :     private use range
/*
/******************************************************************************/
/*****************************************************************************/
int
get_fmodulo (fchar)
  FCHAR fchar;
{

int mod, modfch;


  if ( fchar == '\0' || !isethio(fchar) )
    return ( _S_FAIL );


  if ( isfidel(fchar) ) 
    {
      modfch = UniToMod[fchar-UNIFIDEL];
      if ( modfch < UniToMod[HAE-UNIFIDEL] )
        mod = 8;
      else if ( modfch < UniToMod[QKAE-UNIFIDEL] )
        mod = 12;
      else if ( modfch < UniToMod[MYA-UNIFIDEL] )
        mod = 7;
      else 
        mod = 3;

     /* `he / he correction */
      if ( UniToMod[HAE-UNIFIDEL] <= modfch && modfch <= UniToMod[(HAE+SABI)-UNIFIDEL] ) 
        mod = 7;
      else if ( (UniToMod[HZAE-UNIFIDEL] <= modfch && modfch <= UniToMod[(HZAE+SABI)-UNIFIDEL])
                 ||  (UniToMod[(HZAE+DIQALA_RABI)-UNIFIDEL] <= modfch && modfch <= UniToMod[(HZAE+DIQALA_SADS)-UNIFIDEL]) )
        mod = 12;
     /* we - wo correction //
      else if ( UniToMod[WAE-UNIFIDEL] <= modfch && modfch <= UniToMod[(WAE+SABI)-UNIFIDEL] ) 
        mod = 7; */
    }
  else if ( isfpunct(fchar) ) 
    mod = 15; 
  else if ( isfdigit(fchar) ) 
    mod = 10; 
  else if ( isprivate(fchar) )
    mod = 1; 
  else mod = NIL;

  return mod;

}


 /*****************************************************************************/
/*****************************************************************************/
/*
/*  The following Are Character Classification Routines
/*
/*  Analagous To:  isalfa  -> isfidel 
/*                 isdigit -> isfdigit
/*                 isalnum -> isflnum
/*                 ispunct -> isfpunct
/*                 iscntrl -> isfcntrl  (True for empty address cells)
/*                 isprint -> isfprint  (False if isfcntrl is True)
/*                 isspace -> isfspace  (True for SPACE, ESPACE, TAB, RETURN,
/*                                       NEWLINE, FORMFEED)
/*                 isascii -> isethio
/*                 isgraph -> isfgraph
/*                            isprivate
/*
/*
/******************************************************************************/
/*****************************************************************************/

boolean
isfidel (fchar)
  FCHAR fchar;
{

/*
 *  Note:  The Unicode Fidel detectection doesn't
 *         trap for dead space at this time.
 *
 */


  if ( fchar == '\0' )
    return ( _S_FAIL );

/*if (fchar < ANSI)
    return (isalpha(ch=fchar));
*/
  if ( (HAE <= fchar && fchar <= FYA)               /* base */
       || ( QKAE <= fchar && fchar <= GXAE+ROW+5 )  /* extensions */
       || ( WWAE <= fchar && fchar <= WWAE+5 )      /* private */
     )
    return true;

  return false; 

}


boolean
isfdigit (fchar)
  FCHAR fchar;
{
 

  if ( fchar == '\0' )
    return ( _S_FAIL );

/*if (fchar < ANSI)
    return ( isdigit(ch=fchar) );
*/
  if ( ONE <= fchar && fchar <= TENTHOUSAND )
    return true;

  return false; 

}


boolean
isethio (fchar)
  FCHAR fchar;
{


  if ( fchar == '\0' )
    return ( _S_FAIL );

  if ( UNIFIDEL <= fchar && fchar <= UNIFIDEL+UNITOTAL )
    return true;

  if ( isprivate(fchar) )
    return true;

  return false;

}


boolean
isfpunct (fchar)
  FCHAR fchar;
{


  if ( fchar == '\0' )
    return ( _S_FAIL );

/*if (fchar < ANSI)
    return (ispunct(ch=fchar));
*/
  if ( (SPACE <= fchar && fchar <= PARAGRAPH)
      || (GEMINATION <= fchar && fchar <= EQUESTIONMARK) )
    return true;

  return false;

}


boolean
isflnum (fchar)
  FCHAR fchar;
{


  if ( fchar == '\0' )
    return ( _S_FAIL );

  return ( isfidel(fchar) | isfdigit(fchar) );

}


boolean
isfcntrl (fchar)
  FCHAR fchar;
{


  if ( fchar == '\0' )
    return ( _S_FAIL );

/*if (fchar < ANSI)
    return (iscntrl(ch=fchar));
*/

  if ( fchar == NIL ) 
    return true;


  return false;

}


boolean
isfprint (fchar)
  FCHAR fchar;
{


  if ( fchar == '\0' )
    return ( _S_FAIL );

  return ( !(iscntrl(fchar)) );

}


boolean
isfgraph (fchar)
  FCHAR fchar;
{


  if ( fchar == '\0' )
    return ( _S_FAIL );

/*if (fchar < ANSI)
    return (isgraph(ch=fchar));
*/

 if ( FIDELLAND <= fchar && fchar <= INVAFRICA )
   return true;


  return false;

}


boolean
isfspace (fchar)
  FCHAR fchar;
{

char ch;

  if ( fchar == '\0' )
    return ( _S_FAIL );

  if (fchar < ANSI)
    return ( isspace(ch=fchar) );

  if (fchar == SPACE)
/*if ( fchar == SPACE || fchar == WORDSPACE) */
    return true;

  return false;

}

boolean
isprivate (fchar)
  FCHAR fchar;
{


  if ( fchar == '\0' )
    return ( _S_FAIL );

  if ( PRIVATE_USE_BEGIN <= fchar && fchar <= PRIVATE_USE_END )
    return true;

  return false;

}


