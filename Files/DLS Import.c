#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include <Carbon/Carbon.h>
#include "Undo.h"
#include <QuickTime/QuickTime.h>

#include "dlsmac.h"
#include "dls.h"
#include "dls2.h"
#include "WAV.h"

void ConvertInstrument( register	Byte	*tempPtr,	register long sSize);
void NInitSmallPiano( Rect mainRect, Rect *listRect);
void DrawSmallPianoKey( short i, short color, Rect aRect);
void NDoPlayInstru(short	Note, short Instru, short effect, short arg, short vol);
void ConvertInstrumentIn( register	Byte	*tempPtr,	register long sSize);
short OpenResFileQK( long dirID, short VRefNum);
Handle NSndToHandle( Handle sound, long *loopStart, long *loopEnd, short *sampleSize, unsigned long *sampleRate, long *baseFreq, Boolean *stereo);
void InitMapQK25();

static NoteRequest					myNoteRequest;

/**** Resource Format ****/
typedef struct
{
	long	from;
	long	to;
	
	long	type;
	long	resID;
	long	e;
	long	f;
	
} QuictimeSs;

typedef struct
{
	char			unused[ 0x62];
	short			no;
	QuictimeSs		Ss[];
	
} QuicktimeInst;

/*************************/

/**** Resource Format QK25 ****/

typedef struct
{
	long	size;
	OSType	type;
	long	id;
	long	a[ 2];
	
	Byte	data[];
	
} QuictimeRsrc25;

/*typedef struct
{
	char			unused[ 0x70];		// 0x58
	Str31			name;
	QuictimeSs   	Ss[];
	
} QuicktimeInst25;*/

/*************************/

enum
{
	normalKey = 1,
	redKey,
	greenKey
};

static	MenuHandle		QKInstruMenu[ 18];
static	Boolean			QK50;
static	short			QKInstruSndID[ 18][ 9];
extern	short			LastCanal;
static	short			vers1, vers2;
static	short			QK25Resources[ 500];


OSErr CreateNameListQuicktime25(void)
{
	OSErr					iErr;
	short					foundVRefNum, iFileRef, i, x, GM, z;
	long					foundDirID;
	Handle					hRsrc;
//	QuicktimeInst25			*QuickInst;
	Ptr						tPtr;

	iErr = FindFolder( kOnSystemDisk, kExtensionFolderType, kDontCreateFolder, &foundVRefNum, &foundDirID);
	
	if (iErr == noErr)
	{
		HSetVol( NULL, foundVRefNum, foundDirID);	
		
		iFileRef = OpenResFileQK( foundDirID, foundVRefNum);
		if (iFileRef != -1)
		{
			UseResFile( iFileRef);
			
			x = 0;
			GM = 1;
			for( i = 1; i <= 128; i++)
			{
				if (((i-1) % 8) == 0)
				{
					x++;
					QKInstruMenu[ x] = NewMenu( x, "\p");
					
					GM = 1;
				}
				
				hRsrc = GetResource( 'ssai', i);
				if (hRsrc != NULL)
				{
					DetachResource( hRsrc);
					HLock( hRsrc);
					
					if (GetHandleSize( hRsrc) > 600)
					{
						QuictimeRsrc25	*rsrc, *maxrsrc = (QuictimeRsrc25*) ((*hRsrc) + GetHandleSize( hRsrc));
						
						rsrc = (QuictimeRsrc25*) ((*hRsrc) + 0x20);
						
						while( rsrc < maxrsrc && rsrc->type != kaiToneDescType) rsrc = (QuictimeRsrc25*) (((Ptr) rsrc) + rsrc->size);			// Find the name
						
						tPtr = (Ptr) rsrc->data + 36;
						
						for( z = 1; z <= tPtr[ 0]; z++)
						{
							if (tPtr[ z] == '(') tPtr[ z] = ' ';
							if (tPtr[ z] == ')') tPtr[ z] = ' ';
						}
						
						AppendMenu( QKInstruMenu[ x], (unsigned char*) tPtr);
						QKInstruSndID[ x][ GM] = i;
						GM++;
					}
					HUnlock( hRsrc);
					DisposeHandle( hRsrc);
				}
			}
			
			/** Drums **/
			
			hRsrc = GetResource( 'ssai', 16385);
			if (hRsrc != NULL)
			{
				QuictimeRsrc25	*rsrc, *maxrsrc = (QuictimeRsrc25*) ((*hRsrc) + GetHandleSize( hRsrc));
				
				rsrc = (QuictimeRsrc25*) ((*hRsrc) + 0x20);
				
				while( rsrc < maxrsrc && rsrc->type != kaiToneDescType) rsrc = (QuictimeRsrc25*) (((Ptr) rsrc) + rsrc->size);			// Find the name
				
				tPtr = (Ptr) rsrc->data + 36;
				
				for( z = 1; z <= tPtr[ 0]; z++)
				{
					if (tPtr[ z] == '(') tPtr[ z] = ' ';
					if (tPtr[ z] == ')') tPtr[ z] = ' ';
				}
				
				x++;
				QKInstruMenu[ x] = NewMenu( x, "\p");
				AppendMenu( QKInstruMenu[ x], (unsigned char*) tPtr);
				QKInstruSndID[ x][ 1] = 16385;
				
				ReleaseResource( hRsrc);
			}
			
		//	InitMapQK25();
			
			CloseResFile( iFileRef);
		}
		else {	Erreur( 72, iFileRef);	return -1;}
	}
	else {	Erreur( 72, -1);	return -1;}
	
	return noErr;
}

OSErr CreateNameListQuicktime(void)
{
	OSErr					iErr;
	short					foundVRefNum, iFileRef, i, x, GM, z;
	long					foundDirID;
	Handle					hRsrc;
	QuicktimeInst			*QuickInst;
	Ptr						tPtr;
	
	if (QK50) return CreateNameListQuicktime25();
	
	iErr = FindFolder( kOnSystemDisk, kExtensionFolderType, kDontCreateFolder, &foundVRefNum, &foundDirID);
	
	if (iErr == noErr)
	{
		HSetVol( NULL, foundVRefNum, foundDirID);	
		
		iFileRef = OpenResFileQK( foundDirID, foundVRefNum);
		if (iFileRef != -1)
		{
			UseResFile( iFileRef);
			
			x = 0;
			GM = 1;
			for( i = 1; i <= 128; i++)
			{
				if (((i-1) % 8) == 0)
				{
					x++;
					QKInstruMenu[ x] = NewMenu( x, "\p");
					
					GM = 1;
				}
				
				hRsrc = GetResource( 'INST', i);
				if (hRsrc != NULL)
				{
					QuickInst = (QuicktimeInst*) *hRsrc;
					tPtr = (Ptr) (((Ptr) &QuickInst->no) + 4L + QuickInst->no * sizeof( QuictimeSs));
					
					for( z = 1; z <= tPtr[ 0]; z++)
					{
						if (tPtr[ z] == '(') tPtr[ z] = ' ';
						if (tPtr[ z] == ')') tPtr[ z] = ' ';
					}
					
					AppendMenu( QKInstruMenu[ x], (unsigned char*) tPtr);
					QKInstruSndID[ x][ GM] = i;
					GM++;
					
					ReleaseResource( hRsrc);
				}
			}
			
			/** Drums **/
			
			hRsrc = GetResource( 'INST', 16385);
			if (hRsrc != NULL)
			{
				QuickInst = (QuicktimeInst*) *hRsrc;
				tPtr = (Ptr) (((Ptr) &QuickInst->no) + 4L + QuickInst->no * sizeof( QuictimeSs));
				
				for( z = 1; z <= tPtr[ 0]; z++)
				{
					if (tPtr[ z] == '(') tPtr[ z] = ' ';
					if (tPtr[ z] == ')') tPtr[ z] = ' ';
				}
				
				x++;
				QKInstruMenu[ x] = NewMenu( x, "\p");
				AppendMenu( QKInstruMenu[ x], (unsigned char*) tPtr);
				QKInstruSndID[ x][ 1] = 16393;
				
				ReleaseResource( hRsrc);
			}
			
			CloseResFile( iFileRef);
		}
		else {	Erreur( 72, iFileRef);	return -1;}
	}
	else {	Erreur( 72, -1);	return -1;}
	
	return noErr;
}

static	Rect	SPianoRect[ NUMBER_NOTES];
static	short	curSPos;

void UpdateQKInstruWindow( DialogPtr theDia)
{
GrafPtr	savePort;

	GetPort( &savePort);
	SetPortDialogPort( theDia);
	
	BeginUpdate( GetDialogWindow( theDia));
	
		DrawDialog( theDia);
		oldFrameButton( theDia);
		
		DrawSmallPianoKey( curSPos, greenKey, SPianoRect[ curSPos]);
		
	EndUpdate( GetDialogWindow( theDia));
	SetPort( savePort);
}

void PressSmallPianoQ( DialogPtr TheDia, InstrData	*inst, short ins)
{
	Point	Mouse;
	Handle	itemHandle;
	short	Position, itemType, i;
	Str255	str;
	Rect	itemRect;
	long	newSize;
	Boolean	firstTime = true;
	
	do
	{
		DoGlobalNull();
		
		GetMouse( &Mouse);
		
		Position = -1;
		for( i = 0; i < NUMBER_NOTES; i++)
		{
			if (PtInRect( Mouse, &SPianoRect[ i]))
			{
				Position = i;
				if (SPianoRect[ i].right - SPianoRect[ i].left == 5) goto OK;
			}
		}
		
		if (Position >= 0)
		{
			OK:
			if (Position != curSPos)
			{
				if (curSPos != -1) DrawSmallPianoKey( curSPos, normalKey, SPianoRect[ curSPos]);
				DrawSmallPianoKey( Position, greenKey, SPianoRect[ Position]);
				curSPos = Position;
				
				MADDriver->chan[ LastCanal].KeyOn = false;
				NDoPlayInstru( curSPos, ins, 0, 0, 0xFF);
				
				str[ 0] = 3;
				str[ 1] = ENote[ curSPos][ 0];
				str[ 2] = ENote[ curSPos][ 1];
				str[ 3] = ENote[ curSPos][ 2];
				
				SetDText( TheDia, 10, str);
				
				firstTime = false;
			}
			else
			{
				if (firstTime)
				{
					firstTime = false;
					NDoPlayInstru( curSPos, ins, 0, 0, 0xFF);
				}
			}
		}
	}while( Button());
	
	MADDriver->chan[ LastCanal].KeyOn = false;
}

void DLSImport(void)
{
	DialogPtr				aDialog;
	short					itemHit, itemType, i;
	char					cstr[ 200];
	Str255					aStr, str;
	Rect					tempRect, itemRect;
	Point					myPt;
	Handle					itemHandle;
	long					mresult;
	short					ins, samp, x;
	
	short					CategoryID;
	short					QKInstruID;
	MenuHandle				CategoryMenu;
	
	InstrData				*inst;
	Handle					instCopy = NULL;
	
	for( i = 0; i < 18; i++) QKInstruMenu[ i] = NULL;
	
	if (CreateNameListQuicktime() != noErr) return;
	
	aDialog = GetNewDialog( 176, NULL, (WindowPtr) -1L);
	SetPortDialogPort( aDialog);
	AutoPosition( aDialog);
	ChangeDialogFont( aDialog);
	ShowWindow( GetDialogWindow( aDialog));
	
	pStrcpy( str, "\pQuicktime version ");
	NumToString( vers1, aStr);	pStrcat( str, aStr);		pStrcat( str, "\p.");
	NumToString( vers2, aStr);	pStrcat( str, aStr);
	
	SetDText( aDialog, 21, str);
	
	GetDialogItem( aDialog, 13, &itemType, &itemHandle, &tempRect);
	NInitSmallPiano( tempRect, SPianoRect);

	CategoryMenu = GetMenu( 152);
	for( i = 1; i <= 16; i++)
	{
		if (CountMenuItems( QKInstruMenu[ i]) < 1)
		{
			DisableMenuItem( CategoryMenu, i);
		}
	}
	
	/////

	CategoryID = 1;
	GetMenuItemText( CategoryMenu, CategoryID, aStr);
	SetDText( aDialog, 7, aStr);	
	
	QKInstruID = 1;
	GetMenuItemText( QKInstruMenu[ CategoryID], QKInstruID, aStr);
	SetDText( aDialog, 8, aStr);
	
	
	inst = &curMusic->fid[ ins];
	
	MADPurgeTrackIfInstru( MADDriver, ins);
	ComputeQuicktimeSound( QKInstruSndID[ CategoryID][ QKInstruID], curMusic->sample, &curMusic->fid[ ins], ins);
	CreateInstruList();		DrawInfoInstrument();
	UpdateSampleWindows();	UpdateInstruMenu();
	
	ComputeInstSize( aStr, inst, ins);		SetDText( aDialog, 14, aStr);
	NumToString( QKInstruSndID[ CategoryID][ QKInstruID], aStr);		SetDText( aDialog, 17, aStr);
	/////
	
	curSPos = 42;
	str[ 0] = 3;
	str[ 1] = ENote[ curSPos][ 0];
	str[ 2] = ENote[ curSPos][ 1];
	str[ 3] = ENote[ curSPos][ 2];
	SetDText( aDialog, 10, str);
	
	REGODIA:

	do
	{
		//ModalDialog( MyDlgFilterDesc, &itemHit);
		MyModalDialog( aDialog, &itemHit);
		
		switch( itemHit)
		{
			case 13:
				PressSmallPianoQ( aDialog, inst, ins);
			break;

			case 5:	// Category menu
				InsertMenu( CategoryMenu, hierMenu );
				GetDialogItem( aDialog, itemHit, &itemType, &itemHandle, &tempRect);
				
				myPt.v = tempRect.top;	myPt.h = tempRect.left;
				LocalToGlobal( &myPt);
				
				SetItemMark( CategoryMenu, CategoryID, 0xa5);
				
				mresult = PopUpMenuSelect(	CategoryMenu,
											myPt.v,
											myPt.h,
											CategoryID);
				
				SetItemMark( CategoryMenu, CategoryID, 0);
				
				if ( HiWord(mresult ) != 0 )
				{
					CategoryID = LoWord( mresult);
					
					GetMenuItemText( CategoryMenu, CategoryID, aStr);
					SetDText( aDialog, 7, aStr);
					
					QKInstruID = 1;
					GetMenuItemText( QKInstruMenu[ CategoryID], QKInstruID, aStr);
					SetDText( aDialog, 8, aStr);
					
					MADPurgeTrackIfInstru( MADDriver, ins);
					ComputeQuicktimeSound( QKInstruSndID[ CategoryID][ QKInstruID], curMusic->sample, &curMusic->fid[ ins], ins);
					CreateInstruList();		DrawInfoInstrument();
					UpdateSampleWindows();	UpdateInstruMenu();

					ComputeInstSize( aStr, inst, ins);		SetDText( aDialog, 14, aStr);
					NumToString( QKInstruSndID[ CategoryID][ QKInstruID], aStr);		SetDText( aDialog, 17, aStr);
				}
				DeleteMenu( GetMenuID( CategoryMenu));
			break;
			
			case 6:	// Instruments menu
				InsertMenu( QKInstruMenu[ CategoryID], hierMenu );
				GetDialogItem( aDialog, itemHit, &itemType, &itemHandle, &tempRect);
				
				myPt.v = tempRect.top;	myPt.h = tempRect.left;
				LocalToGlobal( &myPt);
				
				SetItemMark( QKInstruMenu[ CategoryID], QKInstruID, 0xa5);
				
				mresult = PopUpMenuSelect(	QKInstruMenu[ CategoryID],
											myPt.v,
											myPt.h,
											QKInstruID);
				
				SetItemMark( QKInstruMenu[ CategoryID], QKInstruID, 0);
				
				if ( HiWord(mresult ) != 0 )
				{
					QKInstruID = LoWord( mresult);
					
					GetMenuItemText( QKInstruMenu[ CategoryID], QKInstruID, aStr);
					SetDText( aDialog, 8, aStr);
					
					MADPurgeTrackIfInstru( MADDriver, ins);
					ComputeQuicktimeSound( QKInstruSndID[ CategoryID][ QKInstruID], curMusic->sample, &curMusic->fid[ ins], ins);
					CreateInstruList();		DrawInfoInstrument();
					UpdateSampleWindows();	UpdateInstruMenu();

					ComputeInstSize( aStr, inst, ins);		SetDText( aDialog, 14, aStr);
					NumToString( QKInstruSndID[ CategoryID][ QKInstruID], aStr);		SetDText( aDialog, 17, aStr);
				}
				DeleteMenu( GetMenuID( QKInstruMenu[ CategoryID]));
			break;
			
			case 11:	// Note menu
				InsertMenu( NoteMenu, hierMenu );
				GetDialogItem( aDialog, itemHit, &itemType, &itemHandle, &tempRect);
				
				myPt.v = tempRect.top;	myPt.h = tempRect.left;
				LocalToGlobal( &myPt);
				
				SetItemMark( NoteMenu, curSPos+1, 0xa5);
				
				mresult = PopUpMenuSelect(	NoteMenu,
											myPt.v,
											myPt.h,
											curSPos+1);
				
				SetItemMark( NoteMenu, curSPos+1, 0);
				
				if ( HiWord(mresult ) != 0 )
				{
					curSPos = LoWord( mresult) - 1;
					
					GetNoteString( curSPos, aStr);
					SetDText( aDialog, 10, aStr);
					
					NDoPlayInstru( curSPos, ins, 0, 0, 0xFF);
					MADDriver->chan[ LastCanal].KeyOn = false;
					
					GetDialogItem( aDialog, 13, &itemType, &itemHandle, &tempRect);
					InvalWindowRect( GetDialogWindow( aDialog), &tempRect);
				}
				DeleteMenu( GetMenuID( NoteMenu));
			break;
		}
		
	}while( itemHit != 1 && itemHit != 2);
	
	if (itemHit == 1)
	{
		
	}
	else
	{
		/////
		///// RESTORE PREVIOUS INSTRUMENT
		/////
		
		long	inOutCount;
		short	x, temp;
		
		MADPurgeTrackIfInstru( MADDriver, ins);
		
		MADKillInstrument( curMusic, ins);
		
		BlockMoveData( *instCopy, &curMusic->fid[ ins], sizeof( InstrData));
		inOutCount = sizeof( InstrData);
		
		temp = curMusic->fid[ ins].numSamples;
		
		curMusic->fid[ ins].numSamples = 0;
		
		for( x = 0; x < temp; x++)
		{
			sData	*curData;
			
			if (curMusic != NULL)
			{
				curData = MADCreateSample( curMusic, ins, x);
			}
			else
			{
				curData = (sData*) NewPtrClear( sizeof( sData));
			}
			
			BlockMoveData( (*instCopy) + inOutCount, curData, sizeof( sData));
			inOutCount += sizeof( sData);
			
			curData->data = MyNewPtr( curData->size);
			BlockMoveData( (*instCopy) + inOutCount, curData->data, curData->size);
			inOutCount += curData->size;
		}
	}
	
	HUnlock( instCopy);
	DisposeHandle( instCopy);
	
	CreateInstruList();
	DrawInfoInstrument();
	UpdateSampleWindows();
	UpdateInstruMenu();
	
	ENDDialog:
	
	DisposeDialog( aDialog);

	DisposeMenu( CategoryMenu);
	for( i = 0; i < 18; i++)
	{
		if (QKInstruMenu[ i] != NULL) DisposeMenu( QKInstruMenu[ i]);
	}
	UpdateALLWindow();
}
