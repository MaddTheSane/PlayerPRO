/*
 * ptmid.h: Header file for ptmid.
 *
 * Author: Andrew Scott  (c)opyright 1994
 *
 * Date: 17/11/1993 ver 0.0
 *       8/1/1994   ver 0.1
 *       11/2/1994  ver 0.2
 */

#ifndef _PTMIDH
#define _PTMIDH

#define MAXFILELEN 80
#define PTVER "0.3"
#define MAXSAMPS 64
#define MAXCHANS 32
#define DIVSPERPAT 64

#define C2FREQUENCY 8287
#define MIDDLEC 60

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif

#define ERROR /*perror(szProgram)*/

typedef char *Sz;
typedef char Fn[MAXFILELEN];

typedef struct FX_ FX; /** FX structure represents effects **/
struct FX_ {
	unsigned long delay;
	char eff;
	int param;
	FX *pfx;
};
typedef struct EI_ EI; /** EI structure represents event info **/
struct EI_ {
	unsigned long effect;
	int pitch; /*** If pitch == -1, effect is qpm, else duration ***/
	short inst, vol;
	FX *pfxTail; /*** pfxTail points to tail of FX circular list. ***/
	EI *pei;
};
typedef struct Tune_ Tune; /** Tune structure holds list of events **/
struct Tune_ {
	unsigned long count;
	Tune *ptune;
	EI *pei;
};

typedef struct NRL_ NRL; /** NRL structure represents note record list **/
struct NRL_ {
	int inst, vol;
	FX *pfxTail;
	Tune *ptuneNow;
	NRL *pnrl;
};

typedef struct SI_ SI; /** SI structure represents sample info tree **/
struct SI_ {
	Sz fnSample;
	int pitch, perpitch, sample, bFinetune;
	unsigned freq, wLppos, wLplen;
	SI *psiL, *psiR;
};

typedef struct { /** MS structure represents a mix of samples **/
	char szName[22];
	char bDefvol;
	SI *psi;
} MS;

extern char bDrumch, szId[5];
extern int fNocopy, fQuiet, fExtend, fStats, wPatmax, wMaxchan, wQuantval;
extern int wRgmode, wModfmt;
extern unsigned wQuant;
extern SI *rgpsiDrum[128], **rgppsiIns[129], *psiTree;
extern Sz szTitle, szProgram;
extern MS rgmsDecided[MAXSAMPS];
extern Fn fnSampath;
extern Ptr	ResultText;

Tune *PtuneLoadFn(Sz, short*);

void ResolvePtune(Tune *);

void SavePtunePfile(Tune *, MADMusic *theMAD, MADDriverSettings *init);

int ValidquantSz(Sz);

void PrintPro(Ptr myStr);

void Error(Sz, ...);

void Erreur(short a, short b);

void CreateResult(Ptr);

#endif
