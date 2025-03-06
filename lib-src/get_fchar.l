%{
/*
**  Note, this is a char stream scanner for SERA developed for the libeth
**  project.  The routines here are neither robust nor thoroughly tested.
**  Reporting compilation and run problems to facilitators at Admas Concepts
**  will assist libeth development.  The routines in this module are only
**  those that would need to access f\lex specific entities.
**
*/

/*
**  Lex has a default option of 300 positions for a given state. Since this
**  filter handles 411 characters of Fidel, the Fidel ``state'' is broken into
**  two for transportability.  State FIDEL2 is given to lower frequency letters
**  and exits after the first token is matched.
**
**  The original output default was to be Unicode, however since Fidel
**  extensions for Unicode have been postponed for the time being, the
**  default output is set for Junet which can handle the full set.  A 
**  Unicode mapping table is then required and found in ``fidel.map''.
**
**  Considerations for Flex:  Primarily LATIN, GREEK and other script states
**  could be made exclusive with %x declarations. FIDEL, could then be 
**  eliminated as it would be the default initial state.  Flex specific
**  code is found between #if(n)def FLEX_SCANNER .... #endif .
**
**  Lex does not read reliably between multiple open input streams reliably.
**  Flex solves this deficiency of Lex.  For portability and consistant
**  operation of the library functions on all platforms, we can not exceed
**  any of the Lex capabilities in Flex.
*/



#include <fidel.h>
#include <fidel.map>
#include <greek.h>
#include <libeth.h>

#define NEWSTATE SeraFlags->top->s


/* these next three are required for using lex with strings instead of files */

#ifdef __STDC__
  int my_yyinput ( char* buf,int max_size );
#else
  int my_yyinput ();
#endif /* __STDC__ */

#ifdef FLEX_SCANNER                /* if we are using flex */
# define YY_READ_BUF_SIZE 1        /* read one char at a time like lex */
# undef YY_INPUT
# define YY_INPUT(buf, result, max) (result = my_yyinput(buf, max))
#else                              /*    we are using lex */
# undef input
# undef unput
#endif  /* FLEX_SCANNER */

static char* in_string;

static SERAFlags* SeraFlags;

LS LnS[NUMLANGS+1] = LSDEFS;

int change_state ();

%}

%e 3460
%n 625
%p 6150

%s FIDEL FIDEL2 LATIN GREEK GEEZNUMBER FPUNCT LPUNCT HTML_A HTML_B

punct [-!"#$%&'()*+,./:;<=>?\[\\\]^_`{|}~]
Punct [-!"#$%&'()*+,./:;<=>?\[\]^_`{|}]
notpunct [^\-!"#$%&'()*+,./:;<=>?\[\\\]^_`{|}~]
white [ \n\t]
White [ \n\t\\]
NotWhite [^ \n\t\\]
%%
%{
   if (SeraFlags->other_state)
     BEGIN SeraFlags->other_state;
   else
     BEGIN change_state();

   if (yytext != '\0' && SeraFlags->gspace && yytext[0] == ' ') /* expand this logice so "   " does not fail */
     yytext[0] = ':';
%}


%{ 
      /* Standard Defined Escapes */
%}

\\(<|\\|{notpunct})  { 
              if ( yyleng == 1 )           /* lets make sure this isn't the 
			                                * start of an escape.            */
			    {                          
				  yymore();
				  REJECT;
			    }
              if ( SeraFlags->verbatim )  /* do not ignore following white 
			                                * space                          */
			    {
                  yyless(1);
                  return '\\';
			    }
              if ( yytext[1] == '\\' )     /* this is the slash escape.      */
			    {
                  if ( yyleng > 2 && ( yyleng != 3 && yytext[2] != ' ') )
                    yyless(yyleng-2);
                  else if ( yyleng > 3 && yytext[2] == ' ' )
                    yyless(yyleng-3);
                  return '\\';
			    }
              else
                {
                  if ( SeraFlags->top == SeraFlags->minor )
                    {
                      SeraFlags->top   = SeraFlags->major;
                      SeraFlags->major = SeraFlags->minor;
                      SeraFlags->minor = SeraFlags->top;
 
                    }
                  else   /* 
                         ** we are returning from a \~lang escape 
                         ** and switches are one cycle out of phase
                         */
                    SeraFlags->top = SeraFlags->minor;

                }
 
              if ( yytext[yyleng-1] != ' ' )  /* preserve returns and tabs   */
                yyless(yyleng-1);

              BEGIN change_state() ;
            }
\\!{White}?  {
               SeraFlags->verbatim =! SeraFlags->verbatim;

               if ( yytext[yyleng-1] != ' ' )  /* preserve returns and tabs  */
                 yyless(yyleng-1);
               BEGIN change_state();
             }

%{ 
      /* Application Defined Escapes */
%}

\\~e{White}? {
               if ( SeraFlags->verbatim )
			     REJECT;
               if ( yytext[yyleng-1] != ' ' )  /* preserve returns and tabs */
                 yyless(yyleng-1);
               return FIDELLAND;
             }
\\~E{White}? {
               if ( SeraFlags->verbatim )
			     REJECT;
               if ( yytext[yyleng-1] != ' ' )  /* preserve returns and tabs */
                 yyless(yyleng-1);
               return INVFIDELLAND;
             }
\\~a{White}? {
               if ( SeraFlags->verbatim )
			     REJECT;
               if ( yytext[yyleng-1] != ' ' )  /* preserve returns and tabs */
                 yyless(yyleng-1);
               return AFRICA;
             }
\\~A{White}? {
               if ( SeraFlags->verbatim )
			     REJECT;
               if ( yytext[yyleng-1] != ' ' )  /* preserve returns and tabs */
                 yyless(yyleng-1);
               return INVAFRICA;
             }

%{
   /* \\~[uU]  { underline =! underline;    An Example... } */
   /* \\~[bB]  { bold =! bold;              An Example... } */
%}

%{ 
      /* Application Defined Escapes */
%}

\\~`:{White}?   { 
                  if ( SeraFlags->verbatim )
	 		        REJECT;
                  if ( yytext[yyleng-1] != ' ' )  /* preserve returns */
                    yyless(yyleng-1);
                  SeraFlags->colon  = false; 
                }
\\~-:{White}?   { 
                  if ( SeraFlags->verbatim )
	 		        REJECT;
                  if ( yytext[yyleng-1] != ' ' )  /* preserve returns */
                    yyless(yyleng-1);
                  SeraFlags->colon  = true; 
                }
\\~"?"{White}?  { 
                  if ( SeraFlags->verbatim )
	 		        REJECT;
                  if ( yytext[yyleng-1] != ' ' )  /* preserve returns */
                    yyless(yyleng-1);
                  SeraFlags->qmark  = false; 
                }
\\~`"|"{White}? { 
                  if ( SeraFlags->verbatim )
	 		        REJECT;
                  if ( yytext[yyleng-1] != ' ' )  /* preserve returns */
                    yyless(yyleng-1);
                  SeraFlags->qmark  = true; 
                }

"<html>"|"<HTML>"    { SeraFlags->html   =  true;   REJECT; }
"</html>"|"</HTML>"  { REJECT; SeraFlags->html   =  false; }

\\~am[h]?{White}?  { 
                    if ( SeraFlags->verbatim )
		 		      REJECT;
                    if ( yytext[yyleng-1] != ' ' )  /* preserve returns and tabs */
                      yyless(yyleng-1);
                    if (SeraFlags->top->l == amh)
                      SeraFlags->top   = SeraFlags->minor;
			  		else 
                      SeraFlags->top   =& LnS[amh];
                    BEGIN change_state();
                  }

\\~g[e]?z{White}?  { 
                    if ( SeraFlags->verbatim )
	 		          REJECT;
                    if ( yytext[yyleng-1] != ' ' )  /* preserve returns and tabs */
                      yyless(yyleng-1);
                    if (SeraFlags->top->l == gez)
                      SeraFlags->top   = SeraFlags->minor;
			  		else 
                      SeraFlags->top   =& LnS[gez];
                    BEGIN change_state();
                  }

\\~ti[r]?{White}?  { 
                    if ( SeraFlags->verbatim )
					  REJECT;
                    if ( yytext[yyleng-1] != ' ' )  /* preserve returns and tabs */
                      yyless(yyleng-1);
                    if (SeraFlags->top->l == tir)
                      SeraFlags->top   = SeraFlags->minor;
					else 
                      SeraFlags->top   =& LnS[tir];
                    BEGIN change_state();
                  }

\\~la[t]?{White}?  { 
                    if ( SeraFlags->verbatim )
					  REJECT;
                    if ( yytext[yyleng-1] != ' ' )  /* preserve returns and tabs */
                      yyless(yyleng-1);
                    if (SeraFlags->top->l == lat)
                      SeraFlags->top   = SeraFlags->minor;
					else 
                      SeraFlags->top   =& LnS[lat];
                    BEGIN change_state();
                  }

\\~((gre)|(el)){White}? {
                          if ( SeraFlags->verbatim )
			 		        REJECT;
                          if ( yytext[yyleng-1] != ' ' )  /* preserve returns and tabs */
                            yyless(yyleng-1);
                          if (SeraFlags->top->l == gre)
                            SeraFlags->top   = SeraFlags->minor;
			  		      else 
                            SeraFlags->top   =& LnS[gre];
                          BEGIN change_state();
                       }

\\~({NotWhite}*){white}?  { 
                            if ( SeraFlags->verbatim )  /* do not ignore 
							                              * following white 
			                                              * space           */
			                  {
                                yyless(1);
                                return '\\';
			                  }
							/* ignore, we don't know what it is */ 
                            if ( yytext[yyleng-1] == '\n' 
							     || yytext[yyleng-1] == '\t'
								 || yytext[yyleng-1] == '')
                              yyless(yyleng-1);
                             
		                  }

%{
		/* Punctuation Toggle Cotrols */
%}

\\{Punct} {
            if ( SeraFlags->html && yytext[1] == '<')
               REJECT;
            yyless(1);
            SeraFlags->lastpunct = true;
            if (NEWSTATE == fidel)
              SeraFlags->other_state = LPUNCT;
            else
              SeraFlags->other_state = FPUNCT;
           
            BEGIN SeraFlags->other_state;
          }

<FIDEL>{Punct}     {yyless(0); SeraFlags->lastpunct = true; BEGIN FPUNCT;}
<LPUNCT,FPUNCT>{notpunct}  {
                             yyless(0);
                             SeraFlags->other_state = false;
                             SeraFlags->lastpunct = false;
                             BEGIN change_state();
                           }

%{
		/* Fidel Punctuation */
%}

<FIDEL>" "({notpunct}[^0-9]|`[euUiaAEIoOshkS])  {

  /*
  **  The " "->WORDSPACE replace feature is not a requirement of SERA,
  **  it is a feature of this transcriber.  The rules satisfy aesthetic
  **  reasoning:  Preceding character is not white space (" ", \t), \n,
  **  punctuation, or number.  Following character is not punctuation,
  **  or numbers.   This is the only place ->lastpunct is used, the
  **  extra required for it is likely a failure of my lexing skills.
  */

                 yyless(1);
                 if ( !SeraFlags->gspace ) 
                   if ( SeraFlags->out == jis )
                     return SPACE;
                   else
                     return ' ';

                 if ( SeraFlags->lastchar > SPACE
                      || SeraFlags->lastchar == ' '
                      || SeraFlags->lastchar == '\n'
                      || SeraFlags->lastchar == '\t'
                      || ( SeraFlags->lastchar < ANSI
                           && SeraFlags->lastpunct ) 
                      || SeraFlags->lastchar == '\0'     /* at beginning of file */
                    )
                 return ' ';

                 return WORDSPACE;
               }

<FIDEL>" "     {
                 if ( SeraFlags->out == jis && !SeraFlags->gspace )
                   return SPACE;
                 else
                   return ' ';
               }
<FPUNCT>`:     {return WORDSPACE;}
<FPUNCT>","    {return COMMA;}
<FPUNCT>";"    {return SEMICOLON;}
<FPUNCT>:      {
                 if (!SeraFlags->colon)
                    return WORDSPACE;
                  else
                    return COLON;
               }
<FPUNCT>:-     {return PREFACECOLON;}
<FPUNCT>-:     {return COLON;}
<FPUNCT>::     {return FULLSTOP;}

<FIDEL>[ ]?:[ ]?[^\-|:] {  /* Escaping hyphen makes SGU Lex 1.4 happy... */
                         if (!SeraFlags->colon)
                           {
                             if ( !( yytext[yyleng-1] == ' ' && yytext[yyleng-2] != ' ') )
                               yyless(yyleng-1);

                             return WORDSPACE;
                           }
                         else
                           {
                             if (yyleng == 1)
                               return COLON;
  
                             if (yyleng == 2)
                               if (yytext[1] == ':')
                                 {
                                   yyless(1);
                                   return ' ';
                                 }
                               else
                                 {
                                   yyless(1);
                                   return COLON;
                                 }

                             if (yyleng == 3)
                               {
                                 yyless(1);
                                 return ' ';
                               }
                           }
                       }

<FPUNCT>"."        {return NETEB;}
<FIDEL>"."{white}  {
                      /* return FULLSTOP at end of sentence only */
                      yyless(1);
                      if ( SeraFlags->lastchar >= LAE
                           && SeraFlags->lastchar < SPACE )
                        return FULLSTOP;

                      return NETEB;
                    }
<FPUNCT>:"|":  {return PARAGRAPH;}
<FPUNCT><<     {return LEFTQUOTE;}
<FPUNCT>>>     {return RIGHTQUOTE;}
<FPUNCT>''     {return GEMINATION;}
<FPUNCT>`'     {return SOFTSADIS;}
<FPUNCT>`!     {return TEMHERTESLAQ;}
<FPUNCT>`"|"   {return SOSTNETEB; /* I pray noone uses this... */ }

<FPUNCT>"?"    {
                 if (SeraFlags->qmark)
                   return SOSTNETEB;
                  else
                   return EQUESTIONMARK;
               }
<FPUNCT>`"?"   {
                 if (!SeraFlags->qmark)
                   return SOSTNETEB;
                  else
                   return EQUESTIONMARK;
               }

<FPUNCT>[`']+  {/* ignore extraneous special characters */}



%{
		/* Ethiopic Numbers, Also Universal */
%}

<FIDEL>`[1-9]+         { 
                         yyless(1); 
                         SeraFlags->other_state = GEEZNUMBER;
                         BEGIN SeraFlags->other_state;
                       }

<GEEZNUMBER>[1-9]             {return (ONE + yytext[0]-'1');}
<GEEZNUMBER>[1-9]0            {return (TEN + yytext[0]-'1');}
<GEEZNUMBER>[2-9]00           {yyless(1); return (ONE + yytext[0]-'1');}
<GEEZNUMBER>[1-9]000          {yyless(2); return (TEN + yytext[0]-'1');}

<GEEZNUMBER>[2-9](0000)+      {yyless(1); return (ONE + yytext[0]-'1');}
<GEEZNUMBER>[1-9](0000)+/0    {yyless(2); return (TEN + yytext[0]-'1');}
<GEEZNUMBER>[2-9](0000)+/00   {yyless(1); return (ONE + yytext[0]-'1');}
<GEEZNUMBER>[1-9](0000)+/000  {yyless(2); return (TEN + yytext[0]-'1');}

<GEEZNUMBER>[1]?00/(0000)?    {return (HUNDRED);}
<GEEZNUMBER>[1]?0000/(0000)?  {return (TENTHOUSAND);}

<GEEZNUMBER>[^0-9]     {
                         yyless(0); 
                         SeraFlags->other_state = false;
                         BEGIN change_state();
                       }

%{
		/* The Fidel */
%}

<FIDEL>`[sqhkgS]        | 
<FIDEL>[sqhkgS]2        | 
<FIDEL>[hmMbBqQkKwgfp]W | 
<FIDEL>X|D|G|v|V|W      |
<FIDEL>[mMrRfF]Y        {
                          yyless(0); 
                          BEGIN FIDEL2;
                        }

<FIDEL>he         {return HAE + GIZ;}
<FIDEL>hu         {return HAE + KAIB;}
<FIDEL>hi         {return HAE + SALS;}
<FIDEL>ha         {return HAE + RABI;}
<FIDEL>hE         {return HAE + HAMS;}
<FIDEL>h          {return HAE + SADS;}
<FIDEL>ho         {return HAE + SABI;}

<FIDEL>[lL]e      {return LAE + GIZ;}
<FIDEL>[lL]u      {return LAE + KAIB;}
<FIDEL>[lL]i      {return LAE + SALS;}
<FIDEL>[lL]a      {return LAE + RABI;}
<FIDEL>[lL]E      {return LAE + HAMS;}
<FIDEL>[lL]       {return LAE + SADS;}
<FIDEL>[lL]o      {return LAE + SABI;}
<FIDEL>[lL]Wa     {return LAE + DIQALA;}

<FIDEL>He         {return HHAE + GIZ;}
<FIDEL>Hu         {return HHAE + KAIB;}
<FIDEL>Hi         {return HHAE + SALS;}
<FIDEL>Ha         {return HHAE + RABI;}
<FIDEL>HE         {return HHAE + HAMS;}
<FIDEL>H          {return HHAE + SADS;}
<FIDEL>Ho         {return HHAE + SABI;}
<FIDEL>HWa        {return HHAE + DIQALA;}

<FIDEL>[mM]e      {return MAE + GIZ;}
<FIDEL>[mM]u      {return MAE + KAIB;}
<FIDEL>[mM]i      {return MAE + SALS;}
<FIDEL>[mM]a      {return MAE + RABI;}
<FIDEL>[mM]E      {return MAE + HAMS;}
<FIDEL>[mM]       {return MAE + SADS;}
<FIDEL>[mM]o      {return MAE + SABI;}
<FIDEL2>[mM]Wa    {return MAE + DIQALA;}
<FIDEL2>[mM]We    {return MWAE;}
<FIDEL2>[mM]Wi    {return MWAE +    ROW + ROW;}
<FIDEL2>[mM]WE    {return MWAE + 2*(ROW + ROW);}
<FIDEL2>[mM]W[u]? {return MWAE + 3*(ROW + ROW);}
<FIDEL2>[mM]Ya    {return MYA;}

<FIDEL2>(`s|s2)e  {return SZAE + GIZ;}
<FIDEL2>(`s|s2)u  {return SZAE + KAIB;}
<FIDEL2>(`s|s2)i  {return SZAE + SALS;}
<FIDEL2>(`s|s2)a  {return SZAE + RABI;}
<FIDEL2>(`s|s2)E  {return SZAE + HAMS;}
<FIDEL2>(`s|s2)   {return SZAE + SADS;}
<FIDEL2>(`s|s2)o  {return SZAE + SABI;}
<FIDEL2>(`s|s2)Wa {return SZAE  + DIQALA;}

<FIDEL>[rR]e      {return RAE + GIZ;}
<FIDEL>[rR]u      {return RAE + KAIB;}
<FIDEL>[rR]i      {return RAE + SALS;}
<FIDEL>[rR]a      {return RAE + RABI;}
<FIDEL>[rR]E      {return RAE + HAMS;}
<FIDEL>[rR]       {return RAE + SADS;}
<FIDEL>[rR]o      {return RAE + SABI;}
<FIDEL>[rR]Wa     {return RAE + DIQALA;}
<FIDEL2>[rR]Ya    {return RYA;}

<FIDEL>se      {return SAE + GIZ;}
<FIDEL>su      {return SAE + KAIB;}
<FIDEL>si      {return SAE + SALS;}
<FIDEL>sa      {return SAE + RABI;}
<FIDEL>sE      {return SAE + HAMS;}
<FIDEL>s       {return SAE + SADS;}
<FIDEL>so      {return SAE + SABI;}
<FIDEL>sWa     {return SAE + DIQALA;}

<FIDEL>xe      {return SHAE + GIZ;}
<FIDEL>xu      {return SHAE + KAIB;}
<FIDEL>xi      {return SHAE + SALS;}
<FIDEL>xa      {return SHAE + RABI;}
<FIDEL>xE      {return SHAE + HAMS;}
<FIDEL>x       {return SHAE + SADS;}
<FIDEL>xo      {return SHAE + SABI;}
<FIDEL>xWa     {return SHAE + DIQALA;}

<FIDEL>qe      {return QAE + GIZ;}
<FIDEL>qu      {return QAE + KAIB;}
<FIDEL>qi      {return QAE + SALS;}
<FIDEL>qa      {return QAE + RABI;}
<FIDEL>qE      {return QAE + HAMS;}
<FIDEL>q       {return QAE + SADS;}
<FIDEL>qo      {return QAE + SABI;}
<FIDEL2>qWe    {return QAE + DIQALA_GIZ;}
<FIDEL2>qWi    {return QAE + DIQALA_SALS;}
<FIDEL2>qWa    {return QAE + DIQALA_RABI;}
<FIDEL2>qWE    {return QAE + DIQALA_HAMS;}
<FIDEL2>qW[u]? {return QAE + DIQALA_SADS;}

<FIDEL2>(`q|q2)e     {return QKAE + GIZ;}
<FIDEL2>(`q|q2)u     {return QKAE + KAIB;}
<FIDEL2>(`q|q2)i     {return QKAE + SALS;}
<FIDEL2>(`q|q2)a     {return QKAE + RABI;}
<FIDEL2>(`q|q2)E     {return QKAE + HAMS;}
<FIDEL2>(`q|q2)      {return QKAE + SADS;}
<FIDEL2>(`q|q2)o     {return QKAE + SABI;}

<FIDEL>Qe      {return QXAE + GIZ;}
<FIDEL>Qu      {return QXAE + KAIB;}
<FIDEL>Qi      {return QXAE + SALS;}
<FIDEL>Qa      {return QXAE + RABI;}
<FIDEL>QE      {return QXAE + HAMS;}
<FIDEL>Q       {return QXAE + SADS;}
<FIDEL>Qo      {return QXAE + SABI;}
<FIDEL2>QWe    {return QXAE + DIQALA_GIZ;}
<FIDEL2>QWi    {return QXAE + DIQALA_SALS;}
<FIDEL2>QWa    {return QXAE + DIQALA_RABI;}
<FIDEL2>QWE    {return QXAE + DIQALA_HAMS;}
<FIDEL2>QW[u]? {return QXAE + DIQALA_SADS;}

<FIDEL>[bB]e      {return BAE + GIZ;}
<FIDEL>[bB]u      {return BAE + KAIB;}
<FIDEL>[bB]i      {return BAE + SALS;}
<FIDEL>[bB]a      {return BAE + RABI;}
<FIDEL>[bB]E      {return BAE + HAMS;}
<FIDEL>[bB]       {return BAE + SADS;}
<FIDEL>[bB]o      {return BAE + SABI;}
<FIDEL2>[bB]Wa    {return BAE + DIQALA;}
<FIDEL2>[bB]We    {return BWAE;}
<FIDEL2>[bB]Wi    {return BWAE +    ROW + ROW;}
<FIDEL2>[bB]WE    {return BWAE + 2*(ROW + ROW);}
<FIDEL2>[bB]W[u]? {return BWAE + 3*(ROW + ROW);}

<FIDEL2>[vV]e     {return VAE + GIZ;}
<FIDEL2>[vV]u     {return VAE + KAIB;}
<FIDEL2>[vV]i     {return VAE + SALS;}
<FIDEL2>[vV]a     {return VAE + RABI;}
<FIDEL2>[vV]E     {return VAE + HAMS;}
<FIDEL2>[vV]      {return VAE + SADS;}
<FIDEL2>[vV]o     {return VAE + SABI;}
<FIDEL2>[vV]Wa    {return VAE + DIQALA;}

<FIDEL>te      {return TAE + GIZ;}
<FIDEL>tu      {return TAE + KAIB;}
<FIDEL>ti      {return TAE + SALS;}
<FIDEL>ta      {return TAE + RABI;}
<FIDEL>tE      {return TAE + HAMS;}
<FIDEL>t       {return TAE + SADS;}
<FIDEL>to      {return TAE + SABI;}
<FIDEL>tWa     {return TAE + DIQALA;}

<FIDEL>ce      {return CAE + GIZ;}
<FIDEL>cu      {return CAE + KAIB;}
<FIDEL>ci      {return CAE + SALS;}
<FIDEL>ca      {return CAE + RABI;}
<FIDEL>cE      {return CAE + HAMS;}
<FIDEL>c       {return CAE + SADS;}
<FIDEL>co      {return CAE + SABI;}
<FIDEL>cWa     {return CAE + DIQALA;}

<FIDEL2>(`h|h2)e        {return HZAE + GIZ;}
<FIDEL2>(`h|h2)u        {return HZAE + KAIB;}
<FIDEL2>(`h|h2)i        {return HZAE + SALS;}
<FIDEL2>(`h|h2)a        {return HZAE + RABI;}
<FIDEL2>(`h|h2)E        {return HZAE + HAMS;}
<FIDEL2>(`h|h2)         {return HZAE + SADS;}
<FIDEL2>(`h|h2)o        {return HZAE + SABI;}
<FIDEL2>([`]?h|h2)We    {return HZAE + DIQALA_GIZ;}
<FIDEL2>([`]?h|h2)Wi    {return HZAE + DIQALA_SALS;}
<FIDEL2>([`]?h|h2)Wa    {return HZAE + DIQALA_RABI;}
<FIDEL2>([`]?h|h2)WE    {return HZAE + DIQALA_HAMS;}
<FIDEL2>([`]?h|h2)W[u]? {return HZAE + DIQALA_SADS;}

<FIDEL>ne      {return NAE + GIZ;}
<FIDEL>nu      {return NAE + KAIB;}
<FIDEL>ni      {return NAE + SALS;}
<FIDEL>na      {return NAE + RABI;}
<FIDEL>nE      {return NAE + HAMS;}
<FIDEL>n       {return NAE + SADS;}
<FIDEL>no      {return NAE + SABI;}
<FIDEL>nWa     {return NAE + DIQALA;}

<FIDEL>Ne      {return NYAE + GIZ;}
<FIDEL>Nu      {return NYAE + KAIB;}
<FIDEL>Ni      {return NYAE + SALS;}
<FIDEL>Na      {return NYAE + RABI;}
<FIDEL>NE      {return NYAE + HAMS;}
<FIDEL>N       {return NYAE + SADS;}
<FIDEL>No      {return NYAE + SABI;}
<FIDEL>NWa     {return NYAE + DIQALA;}

<FIDEL>e       {return AE + GIZ;}
<FIDEL>u|U     {return AE + KAIB;}
<FIDEL>i       {return AE + SALS;}
<FIDEL>a       {
                if (SeraFlags->top->l == amh)
                  return AE + GIZ;

                return AE + RABI;
               }
<FIDEL>A       {return AE + RABI;}
<FIDEL>E       {return AE + HAMS;}
<FIDEL>I       {return AE + SADS;}
<FIDEL>o|O     {return AE + SABI;}
<FIDEL>e3      {return AE + DIQALA;}

<FIDEL>ke      {return KAE + GIZ;}
<FIDEL>ku      {return KAE + KAIB;}
<FIDEL>ki      {return KAE + SALS;}
<FIDEL>ka      {return KAE + RABI;}
<FIDEL>kE      {return KAE + HAMS;}
<FIDEL>k       {return KAE + SADS;}
<FIDEL>ko      {return KAE + SABI;}
<FIDEL2>kWe    {return KAE + DIQALA_GIZ;}
<FIDEL2>kWi    {return KAE + DIQALA_SALS;}
<FIDEL2>kWa    {return KAE + DIQALA_RABI;}
<FIDEL2>kWE    {return KAE + DIQALA_HAMS;}
<FIDEL2>kW[u]? {return KAE + DIQALA_SADS;}

<FIDEL>Ke      {return KHAE + GIZ;}
<FIDEL>Ku      {return KHAE + KAIB;}
<FIDEL>Ki      {return KHAE + SALS;}
<FIDEL>Ka      {return KHAE + RABI;}
<FIDEL>KE      {return KHAE + HAMS;}
<FIDEL>K       {return KHAE + SADS;}
<FIDEL>Ko      {return KHAE + SABI;}
<FIDEL2>KWe    {return KHAE + DIQALA_GIZ;}
<FIDEL2>KWi    {return KHAE + DIQALA_SALS;}
<FIDEL2>KWa    {return KHAE + DIQALA_RABI;}
<FIDEL2>KWE    {return KHAE + DIQALA_HAMS;}
<FIDEL2>KW[u]? {return KHAE + DIQALA_SADS;}

<FIDEL>`e|e2         {return OAE + GIZ;}
<FIDEL>`[uU]|[uU]2   {return OAE + KAIB;}
<FIDEL>`i|i2         {return OAE + SALS;}
<FIDEL>`[aA]|[aA]2   {return OAE + RABI;}
<FIDEL>`E|E2         {return OAE + HAMS;}
<FIDEL>`I|I2         {return OAE + SADS;}
<FIDEL>`[oO]|[oO]2   {return OAE + SABI;}

<FIDEL>we       {return WAE + GIZ;}
<FIDEL>wu       {return WAE + KAIB;}
<FIDEL>wi       {return WAE + SALS;}
<FIDEL>wa       {return WAE + RABI;}
<FIDEL>wE       {return WAE + HAMS;}
<FIDEL>w        {return WAE + SADS;}
<FIDEL>[wW]o    {return WAE + SABI;}
<FIDEL2>[w]?We    {return WWAE;}
<FIDEL2>[w]?Wi    {return WWAE + 1;}
<FIDEL2>[w]?Wa    {return WWAE + 2;}
<FIDEL2>[w]?WE    {return WWAE + 3;}
<FIDEL2>[w]?W[u]? {return WWAE + 4;}

<FIDEL>ze       {return ZAE + GIZ;}
<FIDEL>zu       {return ZAE + KAIB;}
<FIDEL>zi       {return ZAE + SALS;}
<FIDEL>za       {return ZAE + RABI;}
<FIDEL>zE       {return ZAE + HAMS;}
<FIDEL>z        {return ZAE + SADS;}
<FIDEL>zo       {return ZAE + SABI;}
<FIDEL>zWa      {return ZAE + DIQALA;}

<FIDEL>Ze       {return ZHAE + GIZ;}
<FIDEL>Zu       {return ZHAE + KAIB;}
<FIDEL>Zi       {return ZHAE + SALS;}
<FIDEL>Za       {return ZHAE + RABI;}
<FIDEL>ZE       {return ZHAE + HAMS;}
<FIDEL>Z        {return ZHAE + SADS;}
<FIDEL>Zo       {return ZHAE + SABI;}
<FIDEL>ZWa      {return ZHAE + DIQALA;}

<FIDEL>[yY]e    {return YAE + GIZ;}
<FIDEL>[yY]u    {return YAE + KAIB;}
<FIDEL>[yY]i    {return YAE + SALS;}
<FIDEL>[yY]a    {return YAE + RABI;}
<FIDEL>[yY]E    {return YAE + HAMS;}
<FIDEL>[yY]     {return YAE + SADS;}
<FIDEL>[yY]o    {return YAE + SABI;}
<FIDEL>[yY]Wa   {return YAE + DIQALA;}

<FIDEL>de       {return DAE + GIZ;}
<FIDEL>du       {return DAE + KAIB;}
<FIDEL>di       {return DAE + SALS;}
<FIDEL>da       {return DAE + RABI;}
<FIDEL>dE       {return DAE + HAMS;}
<FIDEL>d        {return DAE + SADS;}
<FIDEL>do       {return DAE + SABI;}
<FIDEL>dWa      {return DAE + DIQALA;}

<FIDEL2>De      {return DHAE + GIZ;}
<FIDEL2>Du      {return DHAE + KAIB;}
<FIDEL2>Di      {return DHAE + SALS;}
<FIDEL2>Da      {return DHAE + RABI;}
<FIDEL2>DE      {return DHAE + HAMS;}
<FIDEL2>D       {return DHAE + SADS;}
<FIDEL2>Do      {return DHAE + SABI;}
<FIDEL2>DWa     {return DHAE + DIQALA;}

<FIDEL>[jJ]e    {return JAE + GIZ;}
<FIDEL>[jJ]u    {return JAE + KAIB;}
<FIDEL>[jJ]i    {return JAE + SALS;}
<FIDEL>[jJ]a    {return JAE + RABI;}
<FIDEL>[jJ]E    {return JAE + HAMS;}
<FIDEL>[jJ]     {return JAE + SADS;}
<FIDEL>[jJ]o    {return JAE + SABI;}
<FIDEL>[jJ]Wa   {return JAE + DIQALA;}

<FIDEL>ge      {return GAE + GIZ;}
<FIDEL>gu      {return GAE + KAIB;}
<FIDEL>gi      {return GAE + SALS;}
<FIDEL>ga      {return GAE + RABI;}
<FIDEL>gE      {return GAE + HAMS;}
<FIDEL>g       {return GAE + SADS;}
<FIDEL>go      {return GAE + SABI;}
<FIDEL2>gWe    {return GAE + DIQALA_GIZ;}
<FIDEL2>gWi    {return GAE + DIQALA_SALS;}
<FIDEL2>gWa    {return GAE + DIQALA_RABI;}
<FIDEL2>gWE    {return GAE + DIQALA_HAMS;}
<FIDEL2>gW[u]? {return GAE + DIQALA_SADS;}

<FIDEL>Te      {return THAE + GIZ;}
<FIDEL>Tu      {return THAE + KAIB;}
<FIDEL>Ti      {return THAE + SALS;}
<FIDEL>Ta      {return THAE + RABI;}
<FIDEL>TE      {return THAE + HAMS;}
<FIDEL>T       {return THAE + SADS;}
<FIDEL>To      {return THAE + SABI;}
<FIDEL>TWa     {return THAE + DIQALA;}

<FIDEL>Ce      {return CHAE + GIZ;}
<FIDEL>Cu      {return CHAE + KAIB;}
<FIDEL>Ci      {return CHAE + SALS;}
<FIDEL>Ca      {return CHAE + RABI;}
<FIDEL>CE      {return CHAE + HAMS;}
<FIDEL>C       {return CHAE + SADS;}
<FIDEL>Co      {return CHAE + SABI;}
<FIDEL>CWa     {return CHAE + DIQALA;}

<FIDEL>Pe      {return PHAE + GIZ;}
<FIDEL>Pu      {return PHAE + KAIB;}
<FIDEL>Pi      {return PHAE + SALS;}
<FIDEL>Pa      {return PHAE + RABI;}
<FIDEL>PE      {return PHAE + HAMS;}
<FIDEL>P       {return PHAE + SADS;}
<FIDEL>Po      {return PHAE + SABI;}
<FIDEL>PWa     {return PHAE + DIQALA;}

<FIDEL>Se      {return TSAE + GIZ;}
<FIDEL>Su      {return TSAE + KAIB;}
<FIDEL>Si      {return TSAE + SALS;}
<FIDEL>Sa      {return TSAE + RABI;}
<FIDEL>SE      {return TSAE + HAMS;}
<FIDEL>S       {return TSAE + SADS;}
<FIDEL>So      {return TSAE + SABI;}
<FIDEL>SWa     {return TSAE + DIQALA;}

<FIDEL2>(`S|S2)e  {return TSZAE + GIZ;}
<FIDEL2>(`S|S2)u  {return TSZAE + KAIB;}
<FIDEL2>(`S|S2)i  {return TSZAE + SALS;}
<FIDEL2>(`S|S2)a  {return TSZAE + RABI;}
<FIDEL2>(`S|S2)E  {return TSZAE + HAMS;}
<FIDEL2>(`S|S2)   {return TSZAE + SADS;}
<FIDEL2>(`S|S2)o  {return TSZAE + SABI;}
<FIDEL2>(`S|S2)Wa {return TSAE  + DIQALA;}

<FIDEL>[fF]e      {return FAE + GIZ;}
<FIDEL>[fF]u      {return FAE + KAIB;}
<FIDEL>[fF]i      {return FAE + SALS;}
<FIDEL>[fF]a      {return FAE + RABI;}
<FIDEL>[fF]E      {return FAE + HAMS;}
<FIDEL>[fF]       {return FAE + SADS;}
<FIDEL>[fF]o      {return FAE + SABI;}
<FIDEL2>[fF]Wa    {return FAE + DIQALA;}
<FIDEL2>[fF]We    {return FWAE;}
<FIDEL2>[fF]Wi    {return FWAE +    ROW + ROW;}
<FIDEL2>[fF]WE    {return FWAE + 2*(ROW + ROW);}
<FIDEL2>[fF]W[u]? {return FWAE + 3*(ROW + ROW);}
<FIDEL2>[fF]Ya    {return FYA;}

<FIDEL>pe      {return PAE + GIZ;}
<FIDEL>pu      {return PAE + KAIB;}
<FIDEL>pi      {return PAE + SALS;}
<FIDEL>pa      {return PAE + RABI;}
<FIDEL>pE      {return PAE + HAMS;}
<FIDEL>p       {return PAE + SADS;}
<FIDEL>po      {return PAE + SABI;}
<FIDEL2>pWa    {return PAE + DIQALA;}
<FIDEL2>pWe    {return PWAE;}
<FIDEL2>pWi    {return PWAE +    ROW + ROW;}
<FIDEL2>pWE    {return PWAE + 2*(ROW + ROW);}
<FIDEL2>pW[u]? {return PWAE + 3*(ROW + ROW);}

%{
		/* Extensions Not In Initial Unicode Standard for Ethiopic */
%}

<FIDEL2>(`q|q2)e      {return QXAE + GIZ;}
<FIDEL2>(`q|q2)u      {return QXAE + KAIB;}
<FIDEL2>(`q|q2)i      {return QXAE + SALS;}
<FIDEL2>(`q|q2)a      {return QXAE + RABI;}
<FIDEL2>(`q|q2)E      {return QXAE + HAMS;}
<FIDEL2>(`q|q2)       {return QXAE + SADS;}
<FIDEL2>(`q|q2)o      {return QXAE + SABI;}

<FIDEL2>(`k|k2)e      {return KKAE + GIZ;}
<FIDEL2>(`k|k2)u      {return KKAE + KAIB;}
<FIDEL2>(`k|k2)i      {return KKAE + SALS;}
<FIDEL2>(`k|k2)a      {return KKAE + RABI;}
<FIDEL2>(`k|k2)E      {return KKAE + HAMS;}
<FIDEL2>(`k|k2)       {return KKAE + SADS;}
<FIDEL2>(`k|k2)o      {return KKAE + SABI;}

<FIDEL2>Xe      {return XAE + GIZ;}
<FIDEL2>Xu      {return XAE + KAIB;}
<FIDEL2>Xi      {return XAE + SALS;}
<FIDEL2>Xa      {return XAE + RABI;}
<FIDEL2>XE      {return XAE + HAMS;}
<FIDEL2>X       {return XAE + SADS;}
<FIDEL2>Xo      {return XAE + SABI;}

<FIDEL2>Ge      {return GYAE + GIZ;}
<FIDEL2>Gu      {return GYAE + KAIB;}
<FIDEL2>Gi      {return GYAE + SALS;}
<FIDEL2>Ga      {return GYAE + RABI;}
<FIDEL2>GE      {return GYAE + HAMS;}
<FIDEL2>G       {return GYAE + SADS;}
<FIDEL2>Go      {return GYAE + SABI;}
<FIDEL2>GWa     {return GYAE + DIQALA;}
<FIDEL2>GWe     {return GYWAE;}
<FIDEL2>GWi     {return GYWAE +    ROW + ROW;}
<FIDEL2>GWE     {return GYWAE + 2*(ROW + ROW);}
<FIDEL2>GW[u]?  {return GYWAE + 3*(ROW + ROW);}

<FIDEL2>(`g|g2)e      {return GXAE + GIZ;}
<FIDEL2>(`g|g2)u      {return GXAE + KAIB;}
<FIDEL2>(`g|g2)i      {return GXAE + SALS;}
<FIDEL2>(`g|g2)a      {return GXAE + RABI;}
<FIDEL2>(`g|g2)E      {return GXAE + HAMS;}
<FIDEL2>(`g|g2)       {return GXAE + SADS;}
<FIDEL2>(`g|g2)o      {return GXAE + SABI;}

%{
		/* Greek Simple Mapping */
%}

%{  /*  this doesn't work...  */
    /* <LATIN,GREEK>{punct}|{notpunct}   { if (!SeraFlags->eth_only) */
    /*            REJECT; }*/
%}
<GREEK>[a-zA-Z] {
                  SeraFlags->lastpunct = false;
                  return (yytext[0] + GREEKBASE);
                }


%{
		/* Simple HTML Control -ignore everything between < > & and ; */
%}

<HTML_A>[^>]  {return yytext[0];}
<HTML_A>>  { 
             yyless(0);  
             SeraFlags->other_state = false;
             BEGIN change_state(); 
           }

"<"        {
             if (SeraFlags->html)
               {
                 yyless(0);
                 SeraFlags->other_state = HTML_A;
                 BEGIN SeraFlags->other_state;
               }
             else
               return '<';
           }

<HTML_B>[^;]  {return yytext[0];}
<HTML_B>;  { 
             yyless(0);  
             SeraFlags->other_state = false;
             BEGIN change_state(); 
           }
&          {
             if (SeraFlags->html)
               {
                 yyless(0);
                 SeraFlags->other_state = HTML_B;
                 BEGIN SeraFlags->other_state;
               }
               else
               return '&';
           }


%{
		/* Anything Else... ...Just Kick It Out */
%}

{Punct}   {
            SeraFlags->lastpunct = true;
            return yytext[0];
          }
%{
        /* To Enable DOS support replace / and * below with % and } */
/*

""      {
            if ( SeraFlags->dos != 1 ) 
              return yytext[0]; 
          }
\n        {
            if ( SeraFlags->dos == 2  ) 
              {
                yyless(0);
                SeraFlags->dos = -2 ;
                return  '';
              }
            if ( SeraFlags->dos == -2  ) 
              SeraFlags->dos = 2 ;
            return yytext[0];
          }
*/
          /* For DOS support, replace * and / above with % and { */
%}

.|\n      {
            SeraFlags->lastpunct = false;
            return yytext[0];
          }

%%




 /*****************************************************************************/
/*****************************************************************************/
/*  get_fchar (tstring, fp, mysflags)
/*
/*  Return address of next token from passed stream of SERA text. Either
/*  the file or text string pointer must be NULL.
/*  The address of the returned token is specified by the character coding
/*  system passed in myflags->out.
/*
/*  get_fchar is intended to be of very low level, such that users would not
/*  be using it.
/*
/*  tstring  -is a character pointer to SERA text
/*  fp       -is a file pointer to SERA text
/*  mysflags -is a SERA Flag structure which MUST contain a valid input and
/*            output type.
/*
/******************************************************************************/
/*****************************************************************************/

FCHAR 
get_fchar (tstring, fp, mysflags)
  char* tstring;
  FILE* fp;
  SERAFlags* mysflags;
{

FCHAR fch;


  SeraFlags = mysflags;

  yyin = fp; 
  in_string = tstring;

 /*  The use of : yy_scan_string(in_string);
  *  for Flex is avoided since it is not backwards
  *  compatible, and is not POSIX compliant.
  */

 /*  Note that ``top'' does NOT need to be reset to   
  *  zero here.  This is true only because sget_fstring
  *  is the only utility for strings that presently   
  *  calls get_fchar which does its own resetting.   
  */                                

  fch = yylex();

  if (fch == EOF)
    fprintf (stderr, "!!EOF!!\n");

  SeraFlags->lastchar = fch;

  SeraFlags = NULL; 
   
  yyin = NULL; 
  in_string = NULL;

  return ( fch );

}

static unsigned top = 0;

 /*****************************************************************************/
/*****************************************************************************/
/*  sget_fstring (target, tstring, mysflags)
/*
/*  Convert a string of SERA text into the requested output character coding 
/*  system.  The addresses of the returned token list is specified by the 
/*  character coding system passed in myflags->out.  The string length of the 
/*  converted letter is returned.  
/*
/*  tstring  -is a character pointer to SERA text
/*  target   -is an address of a FCHAR pointer whom we will direct to the 
/*            the transcribed output.
/*  mysflags -is a SERA Flag structure which MUST contain a valid input and
/*            output type.
/*
/******************************************************************************/
/*****************************************************************************/

int
sget_fstring (target, tstring, mysflags)
  FCHAR** target;
  char* tstring;
  SERAFlags* mysflags;
{

FCHAR* hold;
int count;
register int i;


  top = 0;

  hold = (FCHAR *) malloc ( strlen(tstring) * sizeof (FCHAR) );

  i = 0;
  while ( (hold[i++] = get_fchar(tstring, NULL, mysflags)) );
  count = i-1 ;

  hold[count] = '\0';

  *target = (FCHAR *) malloc ( (count+1) * sizeof (FCHAR) );

  for (i = 0; i <= count; i++ )
    (*target)[i] = hold[i];

  free (hold);

  top = 0;

  return (count);

}


 /*****************************************************************************/
/*****************************************************************************/
/*  fidel_sputc (fch, target, mysflags)
/*
/*  Convert a single address from a supported encoding system into character
/*  types of  another.
/*  The string length of the converted letter is returned.  
/*
/*  fch      -is the passed character address.
/*  target   -is a character pointer whom we will direct to the output 
/*            string allocated from memory by this routine.
/*  mysflags -is a SERA Flag structure which MUST contain a valid input and
/*            output type.
/*
/******************************************************************************/
/*****************************************************************************/

int  
fidel_sputc (fch, target, mysflags)
  FCHAR fch;
  unsigned char** target;
  SERAFlags* mysflags;
{

enum Scripts script;


  if ( 0 < fch && fch < ANSI)
    script = latin;
  else if ( fch >= UNIFIDEL && fch < UNIFIDEL+UNITOTAL )
    script = fidel;
  else if ( fch >= PRIVATE_USE_BEGIN && fch <= PRIVATE_USE_END )
    script = special;
  else                        /* A safe assumption for now... */
    script = greek;


/* We Have The Script and Character Identified, Now Convert To Output Coding */

  switch ( script )
    {

      case latin:

        *target = (char *) malloc ( 2 * sizeof(char) );
        (*target)[0] = fch;
        (*target)[1] = '\0';

        break;


      case greek:

        switch (mysflags->out)
          {
            case uni :
            case jis :
              *target = (char *) malloc ( 3 * sizeof(char) );
              (*target)[0] = JISGreek[fch].ku+160;
              (*target)[1] = JISGreek[fch].ten+160;
              (*target)[2] = '\0';
              break;

            case jun  :
              *target = (char *) malloc ( 3 * sizeof(char) );
              strcpy ( *target, JUNETGreek[fch] );
              break;

            default :
              *target = (char *) malloc ( 2 * sizeof(char) );
              (*target)[0] = fch;
              (*target)[1] = '\0';
              break;
          }
        break;

      case fidel:

        switch ( mysflags->out )
          {
            case uni :
              *target = (char *) malloc ( 3 * sizeof(char) );
              (*target)[0] = (fch >> 8);
              (*target)[1] = (fch & 255);
              (*target)[2] = '\0';
              break;
        
            case jis :
              fch -= UNIFIDEL; 
              *target = (char *) malloc ( 3 * sizeof(char) );
              (*target)[0] = JISFidel[fch].ku + JISADJUST;
              (*target)[1] = JISFidel[fch].ten + JISADJUST;
              (*target)[2] = '\0';
              break;

            case jun :
              fch -= UNIFIDEL;
              *target = (char *) malloc ( strlen(JUNETFidel[fch]) * sizeof(char) );
              strcpy ( *target, JUNETFidel[fch] );
              break;

            case sera :
              fch -= UNIFIDEL;
              *target = (char *) malloc ( strlen(FidelName[fch]) * sizeof(char) );
              strcpy ( *target, FidelName[fch] );
              break;

            case debug : 
              fch -= UNIFIDEL;
              *target = (char *) malloc ( (strlen(FidelName[fch])+2) * sizeof(char) );
              sprintf ( *target, "(%s)", FidelName[fch] );
              break;

            case agafari :
              fch -= UNIFIDEL;
              if (AgafariFidel[fch].next != CNULL)   
                {
                  *target = (char *) malloc ( 3 * sizeof(char) );
                  (*target)[0] = AgafariFidel[fch].first;
                  (*target)[1] = AgafariFidel[fch].next;
                  (*target)[2] = '\0';
                } 
              else 
                {
                  *target = (char *) malloc ( 2 * sizeof(char) );
                  (*target)[0] = AgafariFidel[fch].first;
                  (*target)[1] = '\0';
                } 
              break;

            case alxet :
              fch -= UNIFIDEL;
              if (ALXEtFidel[fch].next != CNULL)   
                {
                  *target = (char *) malloc ( 3 * sizeof(char) );
                  (*target)[0] = ALXEtFidel[fch].first;
                  (*target)[1] = ALXEtFidel[fch].next;
                  (*target)[2] = '\0';
                } 
              else 
                {
                  *target = (char *) malloc ( 2 * sizeof(char) );
                  (*target)[0] = ALXEtFidel[fch].first;
                  (*target)[1] = '\0';
                } 
              break;

            case feedel :
              fch -= UNIFIDEL;
              if (FeedelFidel[fch].next != CNULL)   
                {
                  *target = (char *) malloc ( 3 * sizeof(char) );
                  (*target)[0] = FeedelFidel[fch].first;
                  (*target)[1] = FeedelFidel[fch].next;
                  (*target)[2] = '\0';
                } 
              else
                {
                  *target = (char *) malloc ( 2 * sizeof(char) );
                  (*target)[0] = FeedelFidel[fch].first;
                  (*target)[1] = '\0';
                }
              break;

            case washra :
              fch -= UNIFIDEL;
              if (WashraFidel[fch].first != CNULL)    /* use Washra Primary */
                {
                  *target = (char *) malloc ( 2 * sizeof(char) );
                  (*target)[0] = WashraFidel[fch].first;
                  (*target)[1] = '\0';
                } 
              else if (WashraFidel[fch].next != CNULL)    /* use Washra Secondary */
                {
                  if (mysflags->rtf)
                    {
                      *target = (char *) malloc ( 2 * sizeof(char) );
                      (*target)[0] = WashraFidel[fch].next;
                      (*target)[1] = '\0';
                    }
                  else
                    {
                      *target = (char *) malloc ( 4 * sizeof(char) );
                      sprintf ( *target, "{%c}",  WashraFidel[fch].next );
                    }
                }
              else /* An Undefined Fidel Char By Washra */
                {
                  *target = (char *) malloc ( 1 * sizeof(char) );
                  (*target)[0] = '\0';
                }
              break;

            default  : 
              fprintf(stderr, "Failing Out = %d\n", mysflags->out);
              return ( _S_FAIL );

          }
       break;

      case special:
        switch ( mysflags->out )
          {
            case jis:
              fch  = (JISSIZE-1) + (fch - PRIVATE_USE_END);
              *target = (char *) malloc ( 3 * sizeof(char) );
              (*target)[0] = JISFidel[fch].ku  + JISADJUST;
              (*target)[1] = JISFidel[fch].ten + JISADJUST;
              (*target)[2] = '\0';
              break; 

            case jun:
              fch  = (UNITOTAL-1) + (fch - PRIVATE_USE_END);
              *target = (char *) malloc ( strlen(JUNETFidel[fch]) * sizeof(char) );
              strcpy ( *target, JUNETFidel[fch] );
              break; 

            case sera:
              fch  = (UNITOTAL-1)  + (fch - PRIVATE_USE_END);
              *target = (char *) malloc ( strlen(FidelName[fch]) * sizeof(char) );
              strcpy ( *target, FidelName[fch] );
              break;

            case debug:  
              fch  = (UNITOTAL-1)  + (fch - PRIVATE_USE_END);
              *target = (char *) malloc ( strlen(FidelName[fch]) * sizeof(char) );
              sprintf ( *target, "(%s)", FidelName[fch] );
              break; 

            case agafari:
              fch  = (UNITOTAL-1)  + (fch - PRIVATE_USE_END);
              if (AgafariFidel[fch].next != CNULL)   
                {
                  *target = (char *) malloc ( 3 * sizeof(char) );
                  (*target)[0] = AgafariFidel[fch].first;
                  (*target)[1] = AgafariFidel[fch].next;
                  (*target)[2] = '\0';
                } 
              else
                {
                  *target = (char *) malloc ( 2 * sizeof(char) );
                  (*target)[0] = AgafariFidel[fch].first;
                  (*target)[1] = '\0';
                } 
              break;

            case alxet :
              fch  = (UNITOTAL-1)  + (fch - PRIVATE_USE_END);
              if (ALXEtFidel[fch].next != CNULL)   
                {
                  *target = (char *) malloc ( 3 * sizeof(char) );
                  (*target)[0] = ALXEtFidel[fch].first;
                  (*target)[1] = ALXEtFidel[fch].next;
                  (*target)[2] = '\0';
                } 
              else 
                {
                  *target = (char *) malloc ( 2 * sizeof(char) );
                  (*target)[0] = ALXEtFidel[fch].first;
                  (*target)[1] = '\0';
                } 
              break;

            case feedel:
              fch  = (UNITOTAL-1)  + (fch - PRIVATE_USE_END);
              if (FeedelFidel[fch].next != CNULL)   
                {
                  *target = (char *) malloc ( 3 * sizeof(char) );
                  (*target)[0] = FeedelFidel[fch].first;
                  (*target)[1] = FeedelFidel[fch].next;
                  (*target)[2] = '\0';
                } 
              else
                {
                  *target = (char *) malloc ( 2 * sizeof(char) );
                  (*target)[0] = FeedelFidel[fch].first;
                  (*target)[1] = '\0';
                }
              break;

            case washra :
              fch  = (UNITOTAL-1)  + (fch - PRIVATE_USE_END);
              if (WashraFidel[fch].next == CNULL)    /* use Washra Primary */
                {
                  *target = (char *) malloc ( 2 * sizeof(char) );
                  (*target)[0] = WashraFidel[fch].first;
                  (*target)[1] = '\0';
                } 
              else if (WashraFidel[fch].first == CNULL)    /* use Washra Primary */
                {
                  if (mysflags->rtf)
                    {
                      *target = (char *) malloc ( 2 * sizeof(char) );
                      (*target)[0] = WashraFidel[fch].next;
                      (*target)[1] = '\0';
                    }
                  else
                    {
                      *target = (char *) malloc ( 4 * sizeof(char) );
                      sprintf ( *target, "{%c}",  WashraFidel[fch].next );
                    }
                }
              else /* currently only for << and >> */
                {
                  *target = (char *) malloc ( 3 * sizeof(char) );
                  (*target)[0] = WashraFidel[fch].first;
                  (*target)[1] = WashraFidel[fch].next;
                  (*target)[2] = '\0';
                }
              break;

            default: break;
          }

        break;  

      default:
        *target = (char *) malloc ( 2 * sizeof(char) );
        (*target)[0] = '\0';
        (*target)[1] = '\0';
        break;

  }

  return ( strlen(*target) );
}


/*
**  Simple state changer code required at yylex entry. 
**  Lex may provide a better facility for accomplishing 
**  the same.
*/


int 
change_state()
{

  switch (NEWSTATE)
    {
      case fidel:  return FIDEL;
      case latin:  return LATIN;
      case greek:  return GREEK;

/*   the code should never below this point.
 *   but lets make lint happy and add these cases anyway...
 */
      case special: return FIDEL;
      default:      return FIDEL;
    }


/* we shouldn't get here either... */

  return FIDEL;
}




/*
**  input and unput are required to get lex working with strings instead  
**  of files.  These code segments are from ``lex & yacc'' by John Levine,
**  et al pages 40 - 41.
*/



#ifdef FLEX_SCANNER
int
my_yyinput (buf, max_size)
  char* buf;
  int max_size;
{

int  n, diff;
char* myinputlim;


  if (in_string != NULL)      /*  we are working with a string 
                               *  DO NOT try : if (yyin == NULL)
                               *  Flex defaults yyin to stdin when NULL
                               */
    {
/*
 *    myinputlim = in_string + strlen(in_string);
 *
 *    diff =  myinputlim - &in_string[top] ;
 *
 *    n =  (max_size > diff) ? diff : max_size ;
 */  

      n = strlen(in_string); 
      if (n >= YY_BUF_SIZE)
        n = YY_BUF_SIZE - 1;
      if (top >= n)           
        return 0;             /*  These above five lines may be less elegant
                               *  than what is commented out.  But, it seems
                               *  to accomplish the same and doesn't bone up
                               *  YY_READ_BUF_SIZE is set to 1.  It will 
                               *  crash for strings of size > YY_BUF_SIZE
                               *  with out the size check.
                               *
                               *  For now use this, and figure it out later.
                               */
      if (n > 0)
        {
          memcpy (buf, in_string, n);
          top += n;
        }
   
      return n; 
    }
  else                  /* we are working with a file, use original Flex YY_INPUT */
    {
      if ( yy_current_buffer->yy_is_interactive )  /* if yyin = stdin */
        { 
          int c = '*'; 
          for ( n = 0; n < max_size
                && (c = getc( yyin )) != EOF && c != '\n'; ++n ) 
            buf[n] = (char) c; 
          if ( c == '\n' ) 
            buf[n++] = (char) c; 
          if ( c == EOF && ferror( yyin ) ) 
            YY_FATAL_ERROR( "input in flex scanner failed" ); 
        } 
      else if ( ((n = fread( buf, 1, max_size, yyin )) == 0) 
                && ferror( yyin ) ) 
             YY_FATAL_ERROR( "input in flex scanner failed" );
      return n; 
    }
}

#else

int
input()
{


  if (in_string != NULL)      /* we are working with a string */
    {
      if (in_string[top] == '\0')
         return(0);  /* EOF */

      return (in_string[top++]);
    }
  else   /* we are working with a file, use original Lex input */
    return (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar);

}


void
unput (ch)
  int ch;
{


  if (yyin == NULL)      /* we are working with a string */
    {
      if(ch == 0)
              return;
      if (top) 
        {
          top--;
          return;
        }
    }
  else                   /* we are working with a file, use original Lex unput */
    {
      yytchar = (ch);
      if (yytchar == '\n')
        yylineno--;
      *yysptr++ = yytchar;
    }

  return;

}

#endif  /* FLEX_SCANNER */


 /*****************************************************************************/
/*****************************************************************************/
/*  fidel_notfscanf (target, fp, mysflags)
/*
/*  This routine scans a single string from the file pointed to by ``fp''.
/*  notfscanf is more like fgets sans the requirement for the number of
/*  tokens to be scanned or much like fscanf(fp, "%s", target).
/*
/*  target   -is an FCHAR pointer whom we will direct to an FCHAR output 
/*            string allocated from memory by this routine.
/*  fp       -is a file pointer to SERA text
/*  mysflags -is a SERA Flag structure which MUST contain a valid input and
/*            output type.
/*
/******************************************************************************/
/*****************************************************************************/


int
fidel_notfscanf (target, fp, mysflags)
  FCHAR** target;
  FILE* fp;
  SERAFlags* mysflags;
{

int size;
register int i, j;
FCHAR* temptarget, start, fch;
char text[WSIZE];
IntList* templist = NULL;
IntList root, *rootp;


  if ( fp == NULL || feof(fp) || mysflags == NULL )             /* bail if pointing nowhere */
    return ( _S_FAIL );

  if ( feof(fp) )
    return (0);

/*================================================

  fscanf (fp, "%s", text);

  size = sget_fstring ( &temptarget, text, mysflags ) + start;

  if ( start )
    {
      *target = ( FCHAR * ) malloc ( size * sizeof (FCHAR) );
      (*target)[0] = start; 
      for ( i = 0; i <= size; i++ )
        {
          *(target)[i+1] = temptarget[i];
        }
      free ( temptarget );
    }
  else
    *target = temptarget;

  ================================================*/

  rootp =& root;

  start = check_fidel_ungetc ( fp );               /* reget ungot char, if any */

  if ( start )                                    /* if token returned, add to start of list */
    {
      root.fch = start;
      root.next = NULL;
      start = 1;
    }

  i = start;
  while ( (fch = get_fchar(NULL, fp, mysflags)) 
          &&  fch != EOF 
          &&  fch != ' '
          &&  fch != '\n'
          &&  fch != '\t')  
          
    {
      QueInt ( &rootp, fch );
      i++;
    }

  if (fch != EOF)             /* return terminal token */
   {
    yyin = fp; 
    unput(fch);
    yyin = NULL; 
   }


  if ( i == 0 ) 
    return (i);

  *target = ( FCHAR * ) malloc ( (i+1) * sizeof (FCHAR) );

  rootp =& root;

  for (j = 0; j < i; j++)
    (*target)[j] = PopInt ( &rootp );


  (*target)[j] = '\0';

  return ( i );


}

#ifndef FLEX_SCANNER
FCHAR
fidel_flush (fp, mysflags)
  FILE* fp;
  SERAFlags* mysflags;
{

char* pch;
int i,j;
FCHAR fch;


  if ( *yysptr == NULL )
    return 0;

  pch = yysptr;

  while ( *pch++ )
    i++;

  pch = (char *) malloc ( i * sizeof (char) );

  while ( *yysptr++ )
    pch[i-j] = *yysptr;

  pch[i] = '\0';

  fch = get_fchar (NULL, fp, mysflags);

}

yywrap()
{

  while ( *yysptr++ );

  return ( EOF );

}
#endif  /* FLEX_SCANNER */
