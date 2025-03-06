# Convert SERA Text Into Ethiopic Output Systems

## NOTE
*This is a archived work last updated in 1995, it is not expected to
 build out-of-the-box.  Abundant and better quality solutions are
 available. The PK in this package are available as TrueType and
 Ethiopic text may not be authored directly in Unicode.*


SERA2ANY 0.25
=============

sera2any is a utility for converting Ge'ez text written in SERA into other
useful systems.  Currently supported output types are Unicode, JIS, JUNET,
Java, Agafari, ALXEthiopian, Feedel, and WashRa.

Support for additional MS Windows font systems should be forthcoming. SERA2Any
will hopefully have an X interface soon and should also become an "Any2Any"
type tool.  The purpose of the project is to provide a SERA and Unicode I/O 
utility to make the different MS Windows Ethiopic font systems communicatable.
That is you can import a "WashRa" file into any software when you only have
"Feedel" fonts installed.  Or convert the WashRa file into SERA and email it
to a friend.

An MS Windows Interface would make the tool a great asset to the community.
People knowledgeable in Visual Basic who would like to contribute to this
project please contact yacob@padis.gn.apc.org .


HOW TO USE
----------

After successful installation you may use your favorite ASCII editor to write
a SERA style Latin file.  sera2any can then convert the file into one of the
supported outputs.  sera2any writes output to standard out and error messages
to standard error. Options (below) may be used in combination, 

Generic Usage:

   sera2any [options] filein > fileout

Real Example:

   sera2any -o jis-html -stats -l amh etc/fidel > fidel.html
   sera2any -o washra-rtf -l amh etc/fidel > fidel.rtf


Options Are As Follows:

       -c     Map  ASCII  colon,  ``:'',  to Ge'ez colon (-:) and
              not to the default Ge'ez wordspace (:). Ge'ez  word
              space may now only be given by `: .

       -fromdos
              The input file is DOS text, strip out DOS ^M end of
              line characters.  Output  will  be  in  Unix  text.
              This  option  impacts  parsing  speed  and  is only
              available when sera2any is compiled  for  DOS  sup-
              port.

       -h     Echoes help list to stderr and exits.

       -html  Forces HTML conversion, leaving text between < and >
              or & and ; as Latin.

       -l LL(L)
              Set  starting  language  to that specified by LL or
              LLL where LL(L) is an ISO-639 two or  three  letter
              identifier.  Supported languages are: am(h), ge(z),
              la(t), & ti(r).

       -o system
              Specify output of type system. System  defaults  as
              JUNET  or  may  be  one of: agafari, alxet, feedel, sera,
			  jis, java, unicode, or washra.  Appending -html to the 
			  system forces the scanner to leave as Latin all text 
			  between < and > or & and ;.

              Example:
                -o uni      [Unicode Output]
                -o jis-html [JIS Output With HTML Filtering]

       -q     Map  ASCII  question  mark,  ``?'',  to Ge'ez 3-Dot
              question mark (`?) and not  to  the  default  Ge'ez
              stylized question mark (?).  The use of ? and `? in
              SERA text are now reversed.

       -rtf   Output in Rich Text Format.  Supported for Agafari,
              ALXEthiopian, Feedel, and Washra fonts currently.

       -s     Map ASCII space, `` '', to Ge'ez wordspace (:).

       -stats system
              Provides a Fidel table with statistical  output  of
              character occurrences into the files ``fidel.stats''
              and sorted statistics in ``fidel2.stats''.   System
              defaults  as  sera  or  may  be  one of  junet, jis, 
			  alxet,  agafari,  feedel,  unicode,  or  washra.  
              Appending -html to the system  name  will format the 
			  statistics output for HTML.

              Example: -stats jis-html

       -todos The  input  file is Unix text, insert DOS ^M end of
              line characters.  Output will be in DOS text.  This
              option  impacts parsing speed and is only available
              when sera2any is compiled for DOS support.

       -v     Echo version number and exit.

 

BUGS
----

If bugs are found in the sera2any converter, please open an issue:

	https://github.com/dyacob/sera2latex/issues


SERA
----

Extended documents for The System for Ethiopic Representation in ASCII (SERA)
by World Wide Web at : 

   http://abyssiniagateway.net/fidel/fidel.html
   http://abyssiniagateway.net/fidel/sera-faq.html


The file "etc/fidel" gives an outline of the ASCII system.
Also sera man page are now provided.
Briefly, characters are representation goes like :

     Consonants:
     me   mu   mi   ma   mE   m   mo   mWa

     Independent Vowels:
     e/a*  u/U  i    a/A  E   I   o/O   e3

     Independent Vowels Following a 6th Form Consonant:
              l'e   l'u   l'i   l'a   l'E   l'I   l'o
     also -->       lU          lA          lI    lO

     Consonants With 12 forms:
     hWe  hWu/hW'  hWi   hWa   hWE

     *NOTE:  ``a'' may be used in place of ``e'' for the first lone
             vowel ONLY in Amharic text zones.  See INSTALL for
             setting Amharic as a default language.


