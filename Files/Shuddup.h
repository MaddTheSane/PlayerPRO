#ifndef __SHUDDUPH__
#define __SHUDDUPH__

#define MAXVST 300

#include "RDriver.h"
#include "FileUtils.h"
#include <Carbon/Carbon.h>
#include <QuickTime/QuickTime.h>
#include <stdbool.h>

#define IsCodeOK() false
static inline OSErr CallPlug(short item)
{
	return 0;
}

#pragma pack(push, 2)

typedef struct MIDISettings
{
	bool		MIDIVelocity;
	long		Factor;
	bool		MIDIChannel;
	bool		MIDINoteOff;
} MIDISettings;

enum
{
	InstruTE = 0,
	NoteTE,
	EffectTE,
	ArguTE,
	VolumeTE
};

#define MAXWINDOWS 90

struct OLDMADSpec
{
	OSType		MAD;						// Mad Identification
	char 		name[32];					// Music's name
	char		infos[INFOSSIZE];			// Informations & Author name of the music
	Byte		generalPan;					// General Panning
	Byte		MultiChanNo;				// Number of chan for multichannel
	Byte		MultiChan;					// MultiChannel per tracks?
	long		EPitch;						// New Pitch
	long		ESpeed;						// New Speed
	bool		XMLinear;					// Linear picth table?
	bool		MODMode;					// Limit pitch to MOD pitch table
	bool		showCopyright;				// Show infos at startup? true or false
	Byte		generalPitch;				// General Pitch
	Byte		generalSpeed;				// General Speed
	Byte		generalVol;					// Software general volume
	Byte		numPat;						// Patterns number
	Byte		numChn;						// Channels number
	Byte 		numPointers;				// Partition length
	Byte		numInstru;					// Instruments number
	Byte		numSamples;					// Samples number
	Byte		oPointers[MAXPOINTER];		// Partition : Patterns ID List
	short		speed;						// Default speed
	short		tempo;						// Default tempo
	Byte		chanPan[MAXTRACK];			// Channel settings, from 0 to 256
	Byte		chanVol[MAXTRACK];			// Channel Volume, from 0 to 64
};

typedef struct
{
	bool			PPCMachine;
	bool			AutoCreator;
	short			NoStart;
	short			PianoKey[300];
	short			LoopType;
	//Str32			AutorName;
	RGBColor		tracksColor[96];
	short			PianoPos;
	short			volumeLevel;
	bool			MADCompression;
	//Boolean			SSText;
	//Boolean			SSStars;
	//Boolean			SSJumping;
	//Boolean			FText;
	//Boolean			FStars;
	//Boolean			FBalls;
	bool			ThreadUse;
	//Boolean			FSinScroll;
	bool			RememberMusicList;
	bool			OscilloLine;
	bool			DigitalInstru;
	bool			DigitalNote;
	bool			DigitalEffect;
	bool			DigitalArgu;
	bool			DigitalVol;
	bool			GoToStop;
	short			OscilloSize;
	short			OscilloType;
	bool			ActiveHelp;
	short			SpectrumSize;
	short			SpectrumType;
	
	/** Driver Settings **/
	
	short			numChn;								// Active tracks from 2 to 32, automatically setup when a new music is loaded
	short			outPutBits;							// 8 or 16 Bits
	UnsignedFixed	outPutRate;							// Fixed number, by example : rate44khz, rate22050hz, rate22khz, rate11khz, rate11025hz
	short			outPutMode;							// MonoOutPut, StereoOutPut or DeluxeStereoOutPut ?
	short			driverMode;							// ASCSoundDriver, AWACSoundDriver, MIDISoundDriver or SoundManagerDriver
	bool			antiAliasing;						// Use AntiAliasing filter ?

	/** Mozart Prefs **/

	bool			UseOctaveMarkers;
	bool			UseMarkers;
	short			MarkersSize;
	short			MarkersOffSet;
	short			MozartX;

	/** News 4.5	**/
	
	bool			SpectrumScale;
	bool			ClassicalProjection;
	bool			PianoOctaveMarkers;
	bool			SmallPiano;
	bool			FastMusicList;
	long			FastDigitalEdition;
	
	bool			MacKeyBoard;
	bool			MidiKeyBoard;
	
	bool			MIDIVelocity;
	bool			MIDIChanInsTrack;
	
	short			KeyUpMode;
	
	/** News 4.5.1	**/
	
	long			MicroDelaySize;
	bool			surround;

	/** News 4.5.2	**/

	bool			OCArrow[MAXINSTRU];
	short			MozartC1h, MozartC2h;
	OSType			SoundTypeSamp;
	OSType			SoundTypeIns;
	short			LinesHeight;
	bool			SaveMusicList;
	short			softVolumeLevel;
	
	bool			osciTile;
	
	bool			addExtension;
	
	bool			AutoPlayWhenOpen;
	
	bool			Reverb;
	long			ReverbSize;
	long			ReverbStrength;

	/** News 4.5.3 **/
	
	bool			TickRemover;
	bool			AutomaticOpen;
	bool			FinePositioning;
	
	//short			ChannelType;
	short			amplitude;
	OSType			Compressor;
	Fixed			FrequenceSpeed;
	
	Byte			RecordAllTrack;			// 0 : a track, 1 : all tracks, 2 : current track
	
	/** News 4.5.4	**/
	
	bool			StaffShowAllNotes;
	bool			StaffShowLength;
	short			TempsNum;
	short			TempsUnit;
	short			TrackHeight;
	
	short			FKeyActive[20];
	short			FKeyItem[20];
	short			FKeyWind[20];
	
	/** News 4.5.8 **/
	
	bool			keyMapNote;
	
	/** News 5.0.0 **/
	
	RGBColor		yellC;
	short			whichEditorPatterns;
	bool			MusicTrace;
	long			oversampling;
	
	/** News 5.0.3 **/
	
	short			RAWBits;
	long			RAWRate;
	bool			RAWStereo;
	bool			RAWSigned;
	bool			RAWLittleEndian;
	bool			RAWEOF;
	long			RAWLength;
	long			RAWHeader;
	
	Byte			SelectedTracks[MAXTRACK];
	
	Str32			WinNames[3];
	
	// 5.2
	bool			clickSound;
	bool			patternWrapping;
	bool			SendMIDIClockData;
	short			pianoOffset;
	
	// 5.3
	bool				DontUseFilesMix;
	struct OLDMADSpec	previousSpec;
	
	// 5.5
	
	Point			WinPosO[3][MAXWINDOWS];
	short			WinEtatO[3][MAXWINDOWS];
	short			WinLargO[3][MAXWINDOWS];
	short			WinHiO[3][MAXWINDOWS];
	long			WinIDO[3][MAXWINDOWS];
	Point			WinPos[MAXWINDOWS];
	short			WinEtat[MAXWINDOWS];
	short			WinHi[MAXWINDOWS];
	short			WinLarg[MAXWINDOWS];
	long			WinID[MAXWINDOWS];
	double			Filter[EQPACKET * 2];
	bool			useEQ;
	
	// 5.6
	
	short				lastVisualPlugin;
	bool				editorSoundDrag;
	MADDriverSettings	DirectDriverType;
	short				channelNumber;
	bool				FKeyTracks;
	
	// 5.7
	
	long				Previous_globalEffect[10];
	bool				Previous_globalFXActive;
	long				Previous_chanEffect[MAXTRACK][4];
	FXBus				Previous_chanBus[MAXTRACK];
	FXSets				Previous_Sets[MAXTRACK];
} Prefs;

#pragma pack(pop)

enum {
	eNothing,
	eNoteOFF,
	eStop
};

enum
{
	mTools = 1,
	mOscilloV = 3,
	mSpectrumV = 4,
	mAdap = 6,
	mEqualizer = 7,
	mPatternV = 8,
	mPiano = 9,
	mVisual = 12,
	mMemory = 18,
	mDigitalV = 20,
	mInsV = 22
	//mMusic = 24 - FileMenu ID 9
};

enum {
	RefInstruList 	= 1,
	RefPartition 	= 2,
	RefInstruEdit 	= 3,
	RefBoutons 		= 4,
	RefPatList 		= 5,
	RefAdaptators	= 6,
	RefMODList		= 7,
	RefTools		= 8,
	RefSample		= 9,
	RefPiano		= 10,
	RefClassic		= 11,
	RefPlayer 		= 12,
	RefMozart		= 13,
	RefInstruView	= 14,
	RefDigiView		= 15,
	RefCmd			= 16,
	RefPref			= 17,
	RefTrackView	= 18,
	RefSpectrum		= 20,
	RefParti		= 21,
	RefHelp			= 22,
	RefFindReplace	= 23,
	RefCube			= 24,
	RefActiveHelp	= 25,
	RefWave			= 26,
	RefStaff		= 27,
	RefSndExport	= 28,
	RefQuicktime	= 29,
	RefEqualizer	= 30,
	RefVisual		= 31,
	RefVST			= 32
};

#define deletekey	0x08
#define selectAll	0x16
#define duplicate	0x17
#define getinfo		0x18

#define MyC2PStr MYC2PStr
#define MyP2CStr MYP2CStr
//unsigned char* MyC2PStr(Ptr cStr);
//void MyP2CStr(unsigned char *cStr);
void			NNumToString(short no, Str255 aStr);
void			GetNoteString(short note, Str255	string);
Boolean			GetIns(short *ins, short *samp);
void			DoLoadOtherMusic(Boolean Affiche);
void			Erreur(short ID, OSErr theErr);
void			UpdateAHelpInfo(short *, short , DialogPtr );
void			pStrcat(register unsigned char *s1, register unsigned char *s2);
Boolean			ImportFile(Str255, short, long, OSType );
void			AddMODList(Boolean, Str255 , short , long );
pascal Boolean 	MyDlgFilter(DialogPtr, EventRecord *, short * );
pascal Boolean 	MyDlgFilterSF(DialogPtr , EventRecord *, short *, void *);
pascal Boolean 	mylClickLoop(void);
void 			SwitchColor(short);
void 			SetUpTracksMenu(void);
void			CloseSoundInput(void);
void 			UpdateEditorInfo(void);
void 			UpdatePatListInfo(void);
void 			UpdateMozartInfo(void);
void 			DrawMozartFastPartition(void);
void 			UpdateSampleWindows(void);
void 			UpdateClassicInfo(void);
void 			UpdateInstruMenu(void);
void 			SwitchColorBack(short);
void 			InverseRadio(short, DialogPtr);
void 			OctavesName(short, Ptr	);
void 			SelectCurrentActif();
void 			AddAPattern(void );
short 			GetTracksMenuPos(short );
short 			GetTracksMenuVal(short);
void 			SetUpPartition(short );
void			ExportFile(OSType , FSSpec*);
OSErr 			GereChanged(void );
Boolean 		PageSetUp(void);
void 			Print(void);
void			TurnRadio(short, DialogPtr, Boolean);
void			ForceUpdateALLWindow(void);
void 			PrintBitMap(BitMap);
Ptr				GetAudioChannel(Boolean LeftChannel, long Size);
Boolean 		MyTrackControl(ControlHandle	, Point , ControlActionUPP );
void 			MyMoveControl(ControlHandle ah, short x, short y);
void 			MySizeControl(ControlHandle ah, short x, short y);
Boolean 		MyModalDialog(DialogPtr theDlog, short *itemHit);
short			ConvertCharToNote(char theChar);
OSErr			DoStandardOpen(FSSpec	*spec, Str255 string, OSType inType);
OSErr			DoCustomSave(Str255 bStr, Str255 fileName, OSType theType, FSSpec *spec);
OSErr			inMADPlaySoundData(Ptr soundPtr, long size, long channel, long note, long amplitude, long loopBeg, long loopSize, unsigned long rate, Boolean stereo);
short			GetWhichTrackPlay();
void			ShowCopyrightNote();
void 			GetSizeString(long size, Str255 str, Boolean convert);
OSErr			RAWImportFile(FSSpec	*file);
OSStatus		LaunchURLC(ConstStr255Param urlStr);
void			DoInternetMenu(short theItem);
OSErr MyAEGetDescData(const AEDesc *desc, DescType *typeCode, void *dataBuffer, ByteCount maximumSize, ByteCount *actualSize);
void SetFKeyMode(Boolean v);
OSErr	PPINGetPlugName(OSType kindFile, Str255 name);
void ChangeTracksNo(short NewValue);
void StopAIFFExporting(void);
void RecomputeEQ(void);
void InitPlayWhenClicked(void);
void InitFKeyMenu(void);
void InitPPINPlug(void);
void SetInternetMapping(void);
void DoAIFFExporting(void);
void DoNullParti(void);
void DoNullAdap(void);
void OpenMIDIHardware(void);
void ProcessPanning(Channel *ch, MADDriverRec *intDriver, Boolean);
Boolean CreateAIFFExporting(Boolean OnlyCurrent, short  fRef, FSSpec *newFile, OSType type, FSSpec*);
void InitRecording(void);
OSErr	PPINIdentifyFile(OSType *type, FSSpec	*AlienFile);
void PPINGetFileName(void);
pascal OSStatus CarbonWindowEventHandler(EventHandlerCallRef myHandler, EventRef event, void* userData);
#endif

#ifdef MAINSYSTEM2
		ModalFilterUPP		MyDlgFilterDesc;
		ListClickLoopUPP	mylClickLoopDesc;
		NavEventUPP			MyDlgFilterNavDesc;

		Boolean		gUseControlSize, MusicPlayActive, UseAEErreur;
		short		gScrollBarID, curvRefNum;
		long		MacOSXSystem, curparID, StartTime;
		Boolean		gUseNavigation, Micro, End, Record;
		DialogPtr	ToolsDlog;
		Str63		lastLoadMODListName;
		Cursor		watchCrsr, qdarrow;

		Boolean		mainSystemDrag, Direct, Stereo, StereoMixing, NewSoundManager, NewSoundManager31, Audio16;
		short		theDepth, RowBytes;
		Prefs		thePrefs;
		//Rect		ScreenBounds;
		MenuHandle 	InternetMenu, AppleMenu, NoteMenu, InstruMenu, EffectMenu, TracksNumberMenu, BaseNote, InstruEditMenu, PatternEditMenu;
		MenuHandle	ExportMenu, EditorMenu, FileMenu, VSTMenu, ViewsMenu, EditMenu, WindowMenu, HelpMenu;
		EventRecord	theEvent;

		char		ENote[257][3];
		char		EInstru[MAXINSTRU + 2][3], EArgu[257][2];
		char		EEffect[30];
		MADDriverRec	*MADDriver;
		MADMusic		*curMusic;
		MADLibrary		*gMADLib;
		EventTypeSpec	CarbonWinlist[] = { {kEventClassWindow, kEventWindowClose },
					   { kEventClassWindow, kEventWindowDrawContent },
					   { kEventClassWindow, kEventWindowBoundsChanged } };

#else

extern	EventTypeSpec		CarbonWinlist[];
extern	ModalFilterUPP		MyDlgFilterDesc;
extern	ModalFilterYDUPP	MyDlgFilterSFDesc;
extern	ListClickLoopUPP	mylClickLoopDesc;
extern	NavEventUPP			MyDlgFilterNavDesc;

extern	MADDriverRec	*MADDriver;
extern	MADMusic		*curMusic;
extern	MADLibrary		*gMADLib;

extern	Boolean		gUseControlSize, MusicPlayActive, UseAEErreur;
extern	short		gScrollBarID, VolumeH, SpeedH, PitchH, curvRefNum;
extern	long		MacOSXSystem, curparID, StartTime;
extern	Boolean		gUseNavigation, Micro, End, Record;
extern	DialogPtr	ToolsDlog;
extern	Str63		lastLoadMODListName, curSoundName;
extern	Cursor		watchCrsr, qdarrow;

extern	Boolean		mainSystemDrag, Direct, Stereo, StereoMixing, NewSoundManager, NewSoundManager31, Audio16;
extern	short		theDepth, RowBytes;
extern	Prefs		thePrefs;
//extern	Rect		ScreenBounds;
extern	MenuHandle 	InternetMenu, AppleMenu, NoteMenu, InstruMenu, EffectMenu, TracksNumberMenu, BaseNote, InstruEditMenu, PatternEditMenu;
extern	MenuHandle	ExportMenu, EditorMenu, FileMenu, VSTMenu, ViewsMenu, EditMenu, WindowMenu, HelpMenu;
extern	EventRecord	theEvent;

extern	char		ENote[257][3];
extern	char		EInstru[MAXINSTRU + 2][3], EArgu[257][2];
extern	char		EEffect[30];
#endif

void RollCursor(void);
Boolean QTTypeConversion(OSType fileType);
Boolean CheckFileType(FSSpec theSpec, OSType theType);

__private_extern__ OSErr GetApplicationPackageFSSpecFromBundle(FSSpecPtr theFSSpecPtr);
__private_extern__ OSErr GetApplicationResourceFSSpecFromBundle(FSSpecPtr theFSSpecPtr);

static inline void SwapPcmd(Pcmd *toswap)
{
	if (!toswap) {
		return;
	}
	MOT32(&toswap->structSize);
	MOT16(&toswap->length);
	MOT16(&toswap->posStart);
	MOT16(&toswap->tracks);
	MOT16(&toswap->trackStart);
}
