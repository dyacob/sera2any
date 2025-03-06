

#ifndef GREEK_H
#define GREEK_H  1

#define GREEKBASE 700     /* Greek base point used by lex */

#ifndef HAVE_UNI_GREEK
#define HAVE_UNI_GREEK  1

#define UNIGREEK 0x0370
#define ALPHAOFF 0x0020

#endif  /* GREEKBASE */



#ifndef SUPPORT_JIS_GREEK
#define SUPPORT_JIS_GREEK  1 

#ifndef KUTEN_JIS
#define KUTEN_JIS 1
typedef struct {int ku; int ten; } KUTEN_MAP;
#endif  /* KUTEN_JIS */

#define r3  3
#define r6  6


#define JISGREEK 0x7A21
#ifndef JISADJUST
#define JISADJUST 0xA0
#endif  /* JISADJUST */

KUTEN_MAP JISGreek[52] =
  {
    {r6, 1},{r6, 2},{r6, 3},{r6, 4},{r6, 5},{r6, 6},{r6, 7},{r6, 8},
    {r6, 9},{r3,42},{r6,10},{r6,11},{r6,12},{r6,13},{r6,14},{r6,15},{r6,16},
    {r6,17},{r6,18},{r6,19},{r6,20},{r3,54},{r6,21},{r6,22},{r6,23},{r6,24},

    {r6,33},{r6,34},{r6,35},{r6,36},{r6,37},{r6,38},{r6,39},{r6,40},
    {r6,41},{r3,74},{r6,42},{r6,43},{r6,44},{r6,45},{r6,46},{r6,47},{r6,48},
    {r6,49},{r6,50},{r6,51},{r6,52},{r3,86},{r6,53},{r6,54},{r6,55},{r6,56}
  };

#endif  /* SUPPORT_JIS_GREEK */



#ifndef SUPPORT_JUNET_GREEK
#define SUPPORT_JUNET_GREEK  1

char *JUNETGreek[] =  {

",FA(B"      ,  /* A */
",FB(B"      ,  /* B */
",FW(B"      ,  /* C */
",FD(B"      ,  /* D */
",FE(B"      ,  /* E */
",FV(B"      ,  /* F */
",FC(B"      ,  /* G */
",FG(B"      ,  /* H */
",FI(B"      ,  /* I */
"J"              ,  /* J */
",FJ(B"      ,  /* K */
",FK(B"      ,  /* L */
",FL(B"      ,  /* M */
",FM(B"      ,  /* N */
",FO(B"      ,  /* O */
",FP(B"      ,  /* P */
",FH(B"      ,  /* Q */
",FQ(B"      ,  /* R */
",FS(B"      ,  /* S */
",FT(B"      ,  /* T */
",FU(B"      ,  /* U */
"V"              ,  /* V */
",FY(B"      ,  /* W */
",FN(B"      ,  /* X */
",FX(B"      ,  /* Y */
",FF(B"      ,  /* Z */

",Fa(B"      ,  /* a */
",Fb(B"      ,  /* b */
",Fw(B"      ,  /* c */
",Fd(B"      ,  /* d */
",Fe(B"      ,  /* e */
",Fv(B"      ,  /* f */
",Fc(B"      ,  /* g */
",Fg(B"      ,  /* h */
",Fi(B"      ,  /* i */
"j"              ,  /* j */
",Fj(B"      ,  /* k */
",Fk(B"      ,  /* l */
",Fl(B"      ,  /* m */
",Fm(B"      ,  /* n */
",Fo(B"      ,  /* o */
",Fp(B"      ,  /* p */
",Fh(B"      ,  /* q */
",Fq(B"      ,  /* r */
",Fs(B"      ,  /* s */
",Ft(B"      ,  /* t */
",Fu(B"      ,  /* u */
",Fr(B"      ,  /* v */
",Fy(B"      ,  /* w */
",Fn(B"      ,  /* x */
",Fx(B"      ,  /* y */
",Ff(B"         /* z */
};

#endif  /* SUPPORT_JUNET_GREEK */


#endif  /* GREEK_H */
