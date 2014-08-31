#include <PlayerPROCore/PlayerPROCore.h>
#include <PlayerPROCore/PPPlug.h>
#include <Carbon/Carbon.h>
#include "PTMID.h"
#include "dlsmac.h"
#include "dls.h"
#include "dls2.h"
#include <QuickTime/QuickTime.h>

#include "WAV.h"

//NOTE: A lot of code was copied from MIDI-GM.c.  This file needs to be checked
//to see if it still works like it did.

Handle NSndToHandle(Handle sound, long *loopStart, long *loopEnd, short *sampleSize, unsigned long *sampleRate, long *baseFreq, MADDriverSettings *init);
static NoteRequest		myNoteRequest;
static Boolean			QK50;
Boolean					QuicktimeInstruAvailable;
extern MADMusic			*curMusic;


void ConvertInstrument(Byte *tempPtr, long sSize);
short GenerateDLSFromBundle();
void TESTNEWSYSTEM(sData **sample, InstrData *inst, AtomicInstrument ai);
void Quicktime5(NoteRequest *NoteRequest, sData **sample, InstrData *inst);

//Tip: the original code was written thus:
//long GetNELong(BigEndianLong toget)
//{
//#ifdef __BIG_ENDIAN__
//	return toget;
//#else
//	return EndianS32_BtoL(toget.bigEndianValue);
//#endif
//}
//
//void SetNEOSType(BigEndianOSType *toset, OSType theval)
//{
//#ifdef __BIG_ENDIAN__
//	*toset = theval;
//#else
//	toset->bigEndianValue = EndianU32_LtoB(theval);
//#endif
//}
//As such, you can substitute in your mind what the function would have
//originally looked like


#ifdef __BIG_ENDIAN__
#define GetNELong(toget) toget
#else
#define	GetNELong(toget) EndianS32_BtoL(toget.bigEndianValue)
#endif

#ifdef __BIG_ENDIAN__
#define SetNELong(toset, theval)	*toset = theval
#else
#define SetNELong(toset, theval)	(toset)->bigEndianValue = EndianS32_LtoB(theval)
#endif

#ifdef __BIG_ENDIAN__
#define SetNEOSType(toset, theval)	*toset = theval
#else
#define SetNEOSType(toset, theval)	(toset)->bigEndianValue = EndianU32_LtoB(theval)
#endif

#ifdef __BIG_ENDIAN__
#define GetNEShort(toget) toget
#else
#define GetNEShort(toget) EndianS16_BtoL(toget.bigEndianValue)
#endif

#ifdef __BIG_ENDIAN__
#define GetNEOSType(toget) toget
#else
#define GetNEOSType(toget) EndianU32_BtoL(toget.bigEndianValue)
#endif

#ifdef __BIG_ENDIAN__
#define GetNEUnsignedFixed(toget) toget
#else
#define	GetNEUnsignedFixed(toget) EndianU32_BtoL(toget.bigEndianValue)
#endif

static void ConvertInstrument16(short *tempPtr, long sSize)
{
	short val = 0x8000;
	
	sSize /= 2;
	
	while (sSize > 0) {
		sSize--;
		*(tempPtr++) += val;
	}
}

static inline void ConvertInstrumentIn16(short *tempPtr, long sSize)
{
	ConvertInstrument16(tempPtr, sSize);
}

static void DebugLong(long type)
{
	Str31 str;
	
	MADBE32(&type);
	
	str[0] = 4;
	memcpy(&str[1], &type, 4);
	
	DebugStr(str);
}

/**** Resource Format QK25 ****/

typedef struct {
	long	size;
	OSType	type;
	long	id;
	long	a[2];
	
	Byte	data[];
} QuictimeRsrc25;


typedef struct
{
	long	from;
	long	to;
	
	long	type;
	long	resID;
	long	e;
	long	f;
} QuictimeSs25;

typedef struct {
	char			unused[0x58];
	Str31			name;
	QuictimeSs25	Ss[];
} QuicktimeInst25;

/*************************/


/**** Resource Format ****/
typedef struct {
	long	from;
	long	to;
	
	long	type;
	long	resID;
	long	e;
	long	f;
} QuictimeSs;

typedef struct {
	char			unused[0x62];
	short			no;
	QuictimeSs		Ss[];
} QuicktimeInst;

/*************************/

#pragma mark Atom functions prototypes
static OSErr GetAtomData(MyAtom at, void* data, long size);
static long CountAtomById(MyAtom at, OSType type);
static OSErr FindAtomById(MyAtom at, MyAtom *retat, Boolean LIST, OSType type, short id);
static OSErr GetAtomDataById(MyAtom at, OSType type, void *data, long size);

/*************************/

void pStrcpy(unsigned char *s1, const unsigned char *s2);
static void pStrcat(unsigned char *s1, unsigned char *s2)
{
	unsigned char *p;
	short len, i;
	
	if (*s1+*s2<=255) {
		p = *s1 + s1 + 1;
		*s1 += (len = *s2++);
	} else {
		*s1 = 255;
		p = s1 + 256 - (len = *s2++);
	}
	for (i=len; i; --i)
		*p++ = *s2++;
}


static void OctavesMIDIName(short id, Str255 String)
{
	const char	NNames[][3] = {"C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B "};
	/*	{'Do', 'Do#', 'Ré', 'Ré#', 'Mi', 'Fa', 'Fa#', 'Sol', 'Sol#', 'La', 'La#', 'Si'};	*/
	Str255		WorkStr;
	
	if (id == 0xFF) {
		pStrcpy(String, "\p---");
		return;
	}
	
	NumToString((id / 12), WorkStr);
	String[1] = NNames[(id) % 12][0];
	String[2] = NNames[(id) % 12][1];
	String[0] = 2;
	pStrcat(String, WorkStr);
}

static void SetInstruNameM(short theNo, Str255 theNewName, short MIDIgm, Ptr destName)
{
	short	i;
	Str255	aStr, bStr;
	
	pStrcpy(aStr, "\p(");
	NumToString(MIDIgm, bStr);
	pStrcat(aStr, bStr);
	pStrcat(aStr, "\p) ");
	pStrcat(aStr, theNewName);
	
	for(i = 0; i < 32; i++) {
		if (i < aStr[0])
			destName[i] = aStr[i+1];
		else
			destName[i] = '\0';
	}
}

static void SetSampNameM(Str255 theNewName, Ptr destName)
{
	short i;
	
	for (i = 0; i < 32; i++) {
		if (i < theNewName[0])
			destName[i] = theNewName[i+1];
		else
			destName[i] = '\0';
	}
}

void ComputeQuicktimeSound(short GMInstruID, sData **sample, InstrData* inst, short ins)
{
	short i;
	
	for (i = 0; i < inst->numSamples; i++) {
		if (sample[inst->firstSample + i] != NULL) {
			if (sample[inst->firstSample + i]->data != NULL) {
				free(sample[inst->firstSample + i]->data);
				sample[inst->firstSample + i]->data = NULL;
			}
			free(sample[inst->firstSample + i]);
			sample[inst->firstSample + i] = NULL;
		}
	}
	
	memset(inst, 0, sizeof(InstrData));
	inst->no		= ins;
	inst->volFade	= 900;
	
	/*********/
	
	{
		short 				synthCount;
		OSErr 				iErr;
		NoteAllocator 		na;
		OSType				synthType;
		Str31				synthName;
		AtomicInstrument	ai;
		MusicComponent		mc;
		
		na = OpenDefaultComponent(kNoteAllocatorComponentType, 0);
		
		synthCount = NAGetRegisteredMusicDevice(na, 0, NULL, NULL, NULL, NULL);
		
		while(synthCount) {
			iErr = NAGetRegisteredMusicDevice(na, synthCount, &synthType, synthName, NULL, &mc);
			
			if (!synthName[0]) {
				SynthesizerDescription sd;
				
				MusicGetDescription(mc, &sd);
				memmove(synthName, sd.name, sd.name[0] + 1);
			}
			
			if (synthType == kSoftSynthComponentSubType)
				goto PROCESS;
			
			synthCount--;
		}
		
		goto BAIL;
		
	PROCESS:
		
		// fill out a note request, using NAStuffToneDescription
		
		myNoteRequest.info.flags = 0;
		myNoteRequest.info.midiChannelAssignment = 0;
		*(short *)(&myNoteRequest.info.polyphony)			= EndianS16_NtoB(2);				// simultaneous tones
		*(Fixed *)(&myNoteRequest.info.typicalPolyphony)	= EndianU32_NtoB(0x00010000);
		
		if (GMInstruID < 1)
			GMInstruID = 1;
		
		iErr = NAStuffToneDescription(na, GMInstruID, &myNoteRequest.tone);
		if (iErr != noErr) {
			iErr = NAStuffToneDescription(na, 1, &myNoteRequest.tone);
			if (iErr != noErr) MADDebugStr(__LINE__, __FILE__, "NAStuff ComputeQTSound");
		}
		
		SetNEOSType(&myNoteRequest.tone.synthesizerType, synthType);
		pStrcpy(myNoteRequest.tone.synthesizerName, synthName);
		
		// display the instrument picker dialog box to elicit an instrument from the user
		//iErr = NAPickInstrument(na, NULL, "\pPick an instrument:", &myNoteRequest.tone, kPickSameSynth + kPickUserInsts, 0, 0, 0);
		//if (iErr != noErr) goto BAIL;
		
		iErr = MusicSetPartInstrumentNumber(mc, 1, GetNELong(myNoteRequest.tone.instrumentNumber));
		if (iErr)
			goto BAIL;
		
		iErr = MusicGetPartAtomicInstrument(mc, 1, &ai, 0);
		if (iErr)
			goto BAIL;
		
		/*if (QK50)*/ Quicktime5(&myNoteRequest, sample, inst);
		//		else TESTNEWSYSTEM(sample, inst, ai);
		
		CloseComponent(na);
	}
	
BAIL:
	
	;
}

void InitQuicktimeInstruments(void)
{
	QK50 = true;
	QuicktimeInstruAvailable = true;
}

void Quicktime5(NoteRequest *NoteRequest, sData **sample, InstrData *inst)
{
	FSIORefNum		iFileRef;
	short			ii, i, x;
	OSErr 			iErr = noErr;
	Str255			aStr, bStr;
	sData			*curData;
	Ptr				data;
	CK				ck;
	long			noIns, tot;
	OSType			listType;
	MyAtom			at, sat, insAt, insHe, rgnAt;
	INSTHEADER 		curIns;
	SInt64			tmpPos = 0;
	ByteCount		fSize;
	
	iFileRef = GenerateDLSFromBundle();
	
	if (iErr == noErr) {
		if (iFileRef != -1) {
			// RIFF ID
			fSize = sizeof(ck);
			iErr = FSReadFork(iFileRef, fsAtMark, 0, fSize, &ck, &fSize);
			ck.cksize = EndianU32_LtoN(ck.cksize);
			
			if (ck.ckid != 'RIFF') Debugger();
			
			fSize = sizeof(OSType);
			iErr = FSReadFork(iFileRef, fsAtMark, 0, fSize, &listType, &fSize);
			
			FSGetForkPosition(iFileRef, &tmpPos);
			at.pos = (SInt32)tmpPos;
			at.id = listType;
			at.ref = iFileRef;
			at.size = ck.cksize;
			
			iErr = FindAtomById(at, &sat, true, 'colh', 0);
			if (iErr) DebugLong(iErr);
			
			iErr = GetAtomData(sat, &noIns, sizeof(long));
			if (iErr) DebugLong(iErr);
			noIns = EndianU32_LtoN(noIns);
			
			
			iErr = FindAtomById(at, &sat, true, 'lins', 0);
			if (iErr) DebugLong(iErr);
			
			tot = CountAtomById(sat, 'ins ');
			for (i = 0 ; i < tot; i++) {
				iErr = FindAtomById(sat, &insAt, true, 'ins ', i);
				if (iErr) DebugLong(iErr);
				
				iErr = FindAtomById(insAt, &insHe, true, 'insh', 0);
				if (iErr) DebugLong(iErr);
				
				iErr = GetAtomData(insHe, &curIns, sizeof(curIns));
				if (iErr) DebugLong(iErr);
				
				curIns.cRegions = EndianU32_LtoN(curIns.cRegions);
				curIns.Locale.ulBank = EndianU32_LtoN(curIns.Locale.ulBank);
				curIns.Locale.ulInstrument = EndianU32_LtoN(curIns.Locale.ulInstrument);
				
#if 0
				iErr = FindAtomById(insAt, &InfoAt, true, 'INFO', 0);
				if (iErr) DebugLong(iErr);
				
				iErr = FindAtomById(InfoAt, &InfoData, true, 'INAM', 0);
				if (iErr) DebugLong(iErr);
				
				iErr = GetAtomData(InfoData, insName, sizeof(insName));
#endif
				if (GetNELong(NoteRequest->tone.instrumentNumber) >= 16384) { // DRUM KIT
					if (BitTst(&curIns.Locale.ulBank, 31-31)) {
						long	gmID;
						
						gmID = GetNELong(NoteRequest->tone.instrumentNumber);
						gmID = gmID & 0x000000FF;
						
						if (gmID == curIns.Locale.ulInstrument+1) {
							break;
						}
					}
				} else if (GetNELong(NoteRequest->tone.instrumentNumber) != 0 && GetNELong(NoteRequest->tone.instrumentNumber) > 128) {
					long valeurQT, gmID, valeurBank;
					
					valeurQT = GetNELong(NoteRequest->tone.instrumentNumber);
					valeurQT = valeurQT >> 7;
					
					gmID = GetNELong(NoteRequest->tone.instrumentNumber);
					gmID = gmID & 0x0000007F;
					
					valeurBank = curIns.Locale.ulBank;
					valeurBank = valeurBank & 0x0FFFFFFF;
					valeurBank = valeurBank >> 8;
					
					if (gmID == curIns.Locale.ulInstrument+1) {
						if (valeurQT == valeurBank)
							break;
					}
				} else {
					if (curIns.Locale.ulInstrument+1 == GetNELong(NoteRequest->tone.instrumentNumber))
						break;
				}
			}
			
			if (i < tot) {	// did we find it?
				iErr = FindAtomById(sat, &insAt, true, 'ins ', i);
				if (iErr) DebugLong(iErr);
				
				iErr = FindAtomById(insAt, &insHe, true, 'insh', 0);
				if (iErr) DebugLong(iErr);
				
				iErr = GetAtomData(insHe, &curIns, sizeof(curIns));
				if (iErr) DebugLong(iErr);
				
				curIns.cRegions = EndianU32_LtoN(curIns.cRegions);
				curIns.Locale.ulBank = EndianU32_LtoN(curIns.Locale.ulBank);
				curIns.Locale.ulInstrument = EndianU32_LtoN(curIns.Locale.ulInstrument);
				
#if 0
				iErr = FindAtomById(insAt, &InfoAt, true, 'INFO', 0);
				if (iErr) DebugLong(iErr);
				
				iErr = FindAtomById(InfoAt, &InfoData, true, 'INAM', 0);
				if (iErr) DebugLong(iErr);
				
				iErr = GetAtomData(InfoData, insName, sizeof(insName));
				if (iErr) DebugLong(iErr);
#endif
				
				for (x = 0; x < 32 && x < NoteRequest->tone.instrumentName[0]; x++)
					inst->name[x] = NoteRequest->tone.instrumentName[x+1];
				
				iErr = FindAtomById(insAt, &sat, true, 'lrgn', 0);
				if (iErr) DebugLong(iErr);
				
				for (x = 0; x < curIns.cRegions; x++) {
					RGNHEADER 	rgnh = {0};
					WSMPL 		wsmp = {0};
					WLOOP		loop;
					WAVELINK	wlnk = {0};
					MyAtom 		wvpl, wave;
					WAVEFORMAT	fmt = {0};
					MyAtom 		dataAt;
					
					iErr = FindAtomById(sat, &rgnAt, true, 'rgn ', x);
					if (iErr) DebugLong(iErr);
					
					// Key range
					
					iErr = GetAtomDataById(rgnAt, 'rgnh', &rgnh, sizeof(rgnh));
					if (iErr) DebugLong(iErr);
					
					rgnh.RangeKey.usLow = EndianU16_LtoN(rgnh.RangeKey.usLow);
					rgnh.RangeKey.usHigh = EndianU16_LtoN(rgnh.RangeKey.usHigh);
					rgnh.RangeVelocity.usLow = EndianU16_LtoN(rgnh.RangeVelocity.usLow);
					rgnh.RangeVelocity.usHigh = EndianU16_LtoN(rgnh.RangeVelocity.usHigh);
					rgnh.fusOptions = EndianU16_LtoN(rgnh.fusOptions);
					rgnh.usKeyGroup = EndianU16_LtoN(rgnh.usKeyGroup);
					
					// Wave sample
					
					iErr = GetAtomDataById(rgnAt, 'wsmp', &wsmp, sizeof(wsmp));
					if (iErr) DebugLong(iErr);
					
					wsmp.cbSize = EndianU32_LtoN(wsmp.cbSize);
					wsmp.usUnityNote = EndianU16_LtoN(wsmp.usUnityNote);
					wsmp.sFineTune = EndianU16_LtoN(wsmp.sFineTune);
					wsmp.lAttenuation = EndianU32_LtoN(wsmp.lAttenuation);
					wsmp.fulOptions = EndianU32_LtoN(wsmp.fulOptions);
					wsmp.cSampleLoops = EndianU32_LtoN(wsmp.cSampleLoops);
					if (wsmp.cSampleLoops > 0) {
						ByteCount loopSize = sizeof(loop);
						iErr = FSReadFork(rgnAt.ref, fsAtMark, 0, loopSize, &loop, &loopSize);
						
						loop.cbSize = EndianU32_LtoN(loop.cbSize);
						loop.ulType = EndianU32_LtoN(loop.ulType);
						loop.ulStart = EndianU32_LtoN(loop.ulStart);
						loop.ulLength = EndianU32_LtoN(loop.ulLength);
					} else {
						loop.cbSize = 0;
						loop.ulType = 0;
						loop.ulStart = 0;
						loop.ulLength = 0;
					}
					
					// Wave Data ID
					
					iErr = GetAtomDataById(rgnAt, 'wlnk', &wlnk, sizeof(wlnk));
					if (iErr) DebugLong(iErr);
					
					wlnk.fusOptions = EndianU16_LtoN(wlnk.fusOptions);
					wlnk.usPhaseGroup = EndianU16_LtoN(wlnk.usPhaseGroup);
					wlnk.ulChannel = EndianU32_LtoN(wlnk.ulChannel);
					wlnk.ulTableIndex = EndianU32_LtoN(wlnk.ulTableIndex);
					
					// Load Wave Data
					
					iErr = FindAtomById(at, &wvpl, true, 'wvpl', 0);
					if (iErr) DebugLong(iErr);
					
					iErr = FindAtomById(wvpl, &wave, true, 'wave', wlnk.ulTableIndex);
					if (iErr) DebugLong(iErr);
					
					// FMT
					
					iErr = GetAtomDataById(wave, 'fmt ', &fmt, sizeof(fmt));
					if (iErr) DebugLong(iErr);
					
					fmt.wFormatTag = EndianU16_LtoN(fmt.wFormatTag);
					fmt.nCannels = EndianU16_LtoN(fmt.nCannels);
					fmt.nSamplesPerSec = EndianU32_LtoN(fmt.nSamplesPerSec);
					fmt.nAvgBytesPerSec = EndianU32_LtoN(fmt.nAvgBytesPerSec);
					fmt.nBlockAlign = EndianU16_LtoN(fmt.nBlockAlign);
					fmt.wBitsPerSample = EndianU16_LtoN(fmt.wBitsPerSample);
					
					// Wave Data
					
					iErr = FindAtomById(wave, &dataAt, true, 'data', 0);
					if (iErr) DebugLong(iErr);
					
					data = NewPtr(dataAt.size);
					if (data == NULL) DebugLong(-1);
					iErr = GetAtomDataById(wave, 'data', data, dataAt.size);
					if (iErr) DebugLong(iErr);
					
					// ************************
					// * Add it to instrument *
					// ************************
					
					for (ii = rgnh.RangeKey.usLow - 12; ii <= rgnh.RangeKey.usHigh - 12; ii++) {
						if (ii < NUMBER_NOTES && ii > 0)
							inst->what[ii] = inst->numSamples;
					}
					
					if (curMusic != NULL)
						curData = MADCreateSample(curMusic, inst->no, inst->numSamples);
					else {
						curData = (sData*)calloc(sizeof(sData), 1);
						if (curData == NULL) MADDebugStr(__LINE__, __FILE__, "");
						sample[inst->no * MAXSAMPLE + inst->numSamples] = curData;
						
						inst->numSamples++;
					}
					
					curData->size		= 0;
					curData->loopBeg	= 0;
					curData->loopSize	= 0;
					curData->vol		= MAX_VOLUME;
					curData->loopType	= eClassicLoop;
					curData->amp		= 8;
					curData->relNote	= 60 - wsmp.usUnityNote;
					
					// curData->name
					
					curData->data = malloc(dataAt.size);
					
					if (curData->data == NULL) {
						Erreur(63, -2);
						
						curData->size = 0;
					} else {
						if (dataAt.size)
							memmove(curData->data, data, dataAt.size);
						DisposePtr(data);
						
						switch(fmt.wBitsPerSample)
						{
							case 8:
								ConvertInstrumentIn((Byte*)curData->data, dataAt.size);
								break;
								
							case 16:
								ConvertInstrumentIn16((short*)curData->data, dataAt.size);
								break;
						}
						
						curData->size 		= dataAt.size;
						curData->loopBeg 	= loop.ulStart;
						curData->loopSize 	= loop.ulLength;
						curData->vol 		= 64;
						curData->amp	 	= fmt.wBitsPerSample;
						
						if (fmt.nCannels == 2)
							curData->stereo = true;
						
						if (fmt.nCannels > 2) MADDebugStr(__LINE__, __FILE__, "More than 2 channels");
						
						// **
						curData->c2spd		= fmt.nSamplesPerSec;
						
						if (wsmp.sFineTune != 0)
							curData->c2spd += wsmp.sFineTune * 12.75;		// Si 22050 Hz
						
						// **
						
						OctavesMIDIName(rgnh.RangeKey.usLow - 12, aStr);
						pStrcat(aStr, "\p -> ");
						OctavesMIDIName(rgnh.RangeKey.usHigh - 12, bStr);
						pStrcat(aStr, bStr);
						pStrcat(aStr, "\p, ");
						NumToString(curData->c2spd, bStr);
						pStrcat(aStr, bStr);
						pStrcat(aStr, "\p Hz");
						
						SetSampNameM(aStr, curData->name);
					}
				}
			}
			
			FSCloseFork(iFileRef);
		}
	}
	
BAIL:
	
	return;
}

void TESTNEWSYSTEM(sData **sample, InstrData *inst, AtomicInstrument ai)
{
	short 				no, ii, i;
	OSErr 				iErr;
	Str255				aStr, bStr;
	
	sData				*curData;
	long				size, inOutBytes;
	InstSampleDescRec	*sdesc;
	Ptr					data;
	QTAtom				myKeyRangeInfoAtom = 0;
	QTAtom				mySampleInfoAtom = 0;
	QTAtom				mySampleDescAtom = 0;
	QTAtom				mySampleDataAtom = 0;
	QTAtomID			atomID;
	short				sampleIDMap[500];
	
	for (i = 0; i < 500; i++)
		sampleIDMap[i] = -1;
	
	/***************/
	
	for (i = 0; i < QTCountChildrenOfType(ai, kParentAtomIsContainer, kaiKeyRangeInfoType); i++) {
		myKeyRangeInfoAtom = QTFindChildByIndex(ai, kParentAtomIsContainer, kaiKeyRangeInfoType, i + 1, &atomID);
		
		QTLockContainer(ai);
		
		mySampleDescAtom = QTFindChildByIndex(ai, myKeyRangeInfoAtom, kaiSampleDescType, 1, &atomID);
		
		size = 0;
		iErr = QTGetAtomDataPtr(ai,mySampleDescAtom, &size, (Ptr*)&sdesc);
		if (iErr)
			goto BAIL;
		
		if (sampleIDMap[GetNEShort(sdesc->sampleDataID)] != -1) {
			for (ii = GetNELong(sdesc->pitchLow) - 12; ii <= GetNELong(sdesc->pitchHigh) - 12; ii++) {
				if (ii < NUMBER_NOTES && ii > 0)
					inst->what[ii] = sampleIDMap[GetNEShort(sdesc->sampleDataID)];
			}
		} else {
			sampleIDMap[GetNEShort(sdesc->sampleDataID)] = inst->numSamples;
			
			for (ii = GetNELong(sdesc->pitchLow) - 12; ii <= GetNELong(sdesc->pitchHigh) - 12; ii++) {
				if (ii < NUMBER_NOTES && ii > 0)
					inst->what[ii] = inst->numSamples;
			}
			
			SetInstruNameM(inst->no, myNoteRequest.tone.instrumentName, GetNELong(myNoteRequest.tone.instrumentNumber), inst->name);
			
			mySampleInfoAtom = QTFindChildByID(ai, kParentAtomIsContainer, kaiSampleInfoType, GetNEShort(sdesc->sampleDataID), &no);
			if (mySampleInfoAtom != 0) {
				no = QTCountChildrenOfType(ai, mySampleInfoAtom, kaiSampleDataType);
				if (no != 1) MADDebugStr(__LINE__, __FILE__, "kaiSampleDataType");
				
				mySampleDataAtom = QTFindChildByIndex(ai, mySampleInfoAtom, kaiSampleDataType, 1, &atomID);
				if (mySampleDataAtom == 0) MADDebugStr(__LINE__, __FILE__, "kaiSampleDataType");
				
				size = 0;
				iErr = QTGetAtomDataPtr(ai, mySampleDataAtom, &size, &data);
				if (iErr) MADDebugStr(__LINE__, __FILE__, "QTGetAtomDataPtr");
				
				inOutBytes = (GetNEShort(sdesc->sampleSize) * GetNELong(sdesc->numSamples)) / 8L;
				
				// ************************
				// * Add it to instrument *
				// ************************
				
				{
					if (curMusic != NULL)
						curData = MADCreateSample(curMusic, inst->no, inst->numSamples);
					else {
						curData = (sData*)calloc(sizeof(sData), 1);
						if (curData == NULL) MADDebugStr(__LINE__, __FILE__, "");
						sample[inst->no * MAXSAMPLE + inst->numSamples] = curData;
						
						inst->numSamples++;
					}
					
					curData->size		= 0;
					curData->loopBeg	= 0;
					curData->loopSize	= 0;
					curData->vol		= MAX_VOLUME;
					curData->c2spd		= NOFINETUNE;
					curData->loopType	= eClassicLoop;
					curData->amp		= 8;
					// curData->name
					
					curData->data = malloc(inOutBytes);
					
					if (curData->data == NULL) {
						Erreur(63, -2);
						
						curData->size = 0;
					} else {
						if (inOutBytes)
							memmove(curData->data, data, inOutBytes);
						
						if (GetNEOSType(sdesc->dataFormat) == 'raw ') {
							switch(GetNEShort(sdesc->sampleSize))
							{
								case 8:
									ConvertInstrumentIn((Byte*)curData->data, inOutBytes);
									break;
									
								case 16:
									ConvertInstrumentIn16((short*)curData->data, inOutBytes);
									break;
							}
						}
						
						curData->size 		= inOutBytes;
						curData->loopBeg 	= GetNELong(sdesc->loopStart);
						curData->loopSize 	= GetNELong(sdesc->loopEnd) - GetNELong(sdesc->loopStart);
						curData->vol 		= 64;
						curData->amp	 	= GetNEShort(sdesc->sampleSize);
						
						if (GetNEShort(sdesc->numChannels) == 2)
							curData->stereo = true;
						
						if (GetNEShort(sdesc->numChannels) > 2) MADDebugStr(__LINE__, __FILE__, "More than 2 channels");
						
						// **
						curData->c2spd		= GetNEUnsignedFixed(sdesc->sampleRate) >> 16;
						curData->relNote	= 60 - GetNELong(sdesc->pitchNormal);
						// **
						
						OctavesMIDIName(GetNELong(sdesc->pitchLow) - 12, aStr);
						pStrcat(aStr, "\p -> ");
						OctavesMIDIName(GetNELong(sdesc->pitchHigh) - 12, bStr);
						pStrcat(aStr, bStr);
						pStrcat(aStr, "\p, ");
						NumToString(curData->c2spd, bStr);
						pStrcat(aStr, bStr);
						pStrcat(aStr, "\p Hz");
						
						SetSampNameM(aStr, curData->name);
					}
				}
			}
		}
		QTUnlockContainer(ai);
	}
	
BAIL:
	;
}

FSIORefNum GenerateDLSFromBundle()
{
	CFBundleRef	AudioBundle;
	CFURLRef	bundleURL;
	FSRef 		bundleFSRef, rsrcRef;
	FSSpec		file;
	CFURLRef	rsrcURL;
	FSIORefNum	refNum;
	OSErr		iErr;
	
	iErr = FindFolder(kOnSystemDisk, kComponentsFolderType, kDontCreateFolder, &file.vRefNum, &file.parID);
	if (iErr == noErr) {
		FSMakeFSSpec(file.vRefNum, file.parID, "\pCoreAudio.component", &file);
	} else
		return -1;
	
	FSpMakeFSRef(&file, &bundleFSRef);
	bundleURL = CFURLCreateFromFSRef(kCFAllocatorDefault, &bundleFSRef);
	AudioBundle = CFBundleCreate(kCFAllocatorDefault, bundleURL);
	CFRelease(bundleURL);
	if (AudioBundle == NULL) {
		Debugger();
		return -1;
	}
	
	rsrcURL = CFBundleCopyResourceURL(AudioBundle, CFSTR("gs_instruments"),  CFSTR("dls"), NULL);
	CFURLGetFSRef(rsrcURL, &rsrcRef);
	iErr = FSOpenFork(&rsrcRef, 0, 0, fsRdPerm, &refNum);
	CFRelease(rsrcURL);
	if (iErr != noErr)
		refNum = -1;
	CFRelease(AudioBundle);
	return refNum;
}

#pragma mark Atom functions
static OSErr GetAtomData(MyAtom at, void* data, long size)
{
	ByteCount	fSize;
	CK			sck;
	OSErr		iErr;
	
	iErr = FSSetForkPosition(at.ref, fsFromStart, at.pos - 4);
	
	fSize = sizeof(sck);
	iErr = FSReadFork(at.ref, fsAtMark, 0, fSize, &sck, &fSize);
	if (iErr) Debugger();
	sck.cksize = EndianU32_NtoL(sck.cksize);
	
	fSize = size;
	iErr = FSReadFork(at.ref, fsAtMark, 0, fSize, data, &fSize);
	if (iErr) Debugger();
	
	return noErr;
}

static long CountAtomById(MyAtom at, OSType type)
{
	long		index = 0, listSize = at.size - 4;
	ByteCount	fSize = 0;
	CK			sck;
	OSErr		iErr;
	SInt64		prePos;
	OSType		ilistType;
	
	iErr = FSSetForkPosition(at.ref, fsFromStart, at.pos);
	
	do {
		fSize = sizeof(sck);
		iErr = FSReadFork(at.ref, fsAtMark, 0, fSize, &sck, &fSize);
		if (iErr) DebugLong(iErr);
		sck.cksize = EndianU32_NtoL(sck.cksize);
		
		FSGetForkPosition(at.ref, &prePos);
		
		listSize -= sizeof(sck);
		
		if (sck.ckid == type)
			index++;
		
		switch(sck.ckid) {
			case 'LIST':
				fSize = sizeof(OSType);
				iErr = FSReadFork(at.ref, fsAtMark, 0, fSize, &ilistType, &fSize);
				
				if (ilistType == type)
					index++;
				break;
		}
		
		iErr = FSSetForkPosition(at.ref, fsFromStart, prePos + sck.cksize);
		if (iErr) DebugLong(iErr);
		
		listSize -= sck.cksize;
		
		listSize /= 2;
		listSize *= 2;
		
	} while(iErr == noErr && listSize > 0);
	
	if (listSize < 0) Debugger();
	
	return index;
}

static OSErr FindAtomById(MyAtom at, MyAtom *retat, Boolean LIST, OSType type, short id)
{
	OSType		ilistType;
	long		index = 0, listSize = at.size - 4;
	CK			sck;
	OSErr		iErr;
	ByteCount	fSize;
	SInt64		prePos;
	
	iErr = FSSetForkPosition(at.ref, fsFromStart, at.pos);
	
	do {
		fSize = sizeof(sck);
		iErr = FSReadFork(at.ref, fsAtMark, 0, fSize, &sck, &fSize);
		if (iErr) DebugLong(iErr);
		sck.cksize = EndianU32_NtoL(sck.cksize);
		
		FSGetForkPosition(at.ref, &prePos);
		
		listSize -= sizeof(sck);
		
		if (sck.ckid == type) {
			if (index == id) {
				// We found it !!!!!
				retat->pos = (SInt32)(prePos - 4);
				retat->id = sck.ckid;
				retat->size = sck.cksize;
				retat->ref = at.ref;
				
				return noErr;
			}
			index++;
		}
		
		switch (sck.ckid) {
			case 'LIST':
				//case 'INFO':
				fSize = sizeof(OSType);
				iErr = FSReadFork(at.ref, fsAtMark, 0, fSize, &ilistType, &fSize);
				
				if (ilistType == type) {
					if (index == id) {
						FSGetForkPosition(at.ref, &prePos);
						
						// We found it !!!!!
						retat->pos = (SInt32)prePos;
						retat->id = ilistType;
						retat->size = sck.cksize;
						retat->ref = at.ref;
						
						return noErr;
					}
					index++;
				}
				break;
		}
		
		iErr = FSSetForkPosition(at.ref, fsFromStart, prePos + sck.cksize);
		if (iErr) DebugLong(iErr);
		
		listSize -= sck.cksize;
		
		listSize /= 2;
		listSize *= 2;
	} while(iErr == noErr && listSize > 0);
	
	if (listSize < 0) Debugger();
	
	return -1;
}

static OSErr GetAtomDataById(MyAtom at, OSType type, void *data, long size)
{
	MyAtom 	tempAt;
	OSErr	iErr;
	
	iErr = FindAtomById(at, &tempAt, true, type, 0);
	if (iErr)
		return iErr;
	
	
	iErr = GetAtomData(tempAt, data, size);
	if (iErr)
		return iErr;
	
	return noErr;
}
