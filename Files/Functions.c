#include "Shuddup.h"
#include "RDriver.h"
#include "Undo.h"

void TurnRadio( short	item, DialogPtr	dlog, Boolean alors);
void SetDText (DialogPtr dlog, short item, Str255 str);
void ControlSwitch(short	item, DialogPtr	dlog, short	Switch);
void OutPutHexShort( Str255 outch, short column, short shortNumber);
void FrameButton(DialogPtr theDia, short whichButton, Boolean drawIt);
void oldFrameButton( DialogPtr theDlg);
void SetSelectionEditor( short FPat, short FPos, short FTrack);
Boolean SetUpFind();
Cmd * GetEditorSelectCmd( short *track, short *position);
void DoItemPressFind( short whichItem, DialogPtr whichDialog);
void GetDigitalSelection( short *XStart, short *YStart, short *XEnd, short *YEnd, short*);
void SetDigitalSelection( short XStart, short YStart, short XEnd, short YEnd);
void GetDText (DialogPtr dlog, short item, StringPtr str);
void Erreur( short ID, OSErr	theErr);
	
		DialogPtr		FindDlog;
extern	DialogPtr		MozartDlog, EditorDlog;
extern	MenuHandle		EditorMenu;
extern	WindowPtr		oldWindow;
extern	MenuHandle		TrackView;

enum
{
	CurrentPat = 0,
	DigitalSelection,
	ClassicalSelection,
	Everywhere
};

enum
{
	Find = 0,
	Next,
	ReplaceFind,
	ReplaceAll
};

		
static	short		FindInstru,  FindNote,  FindEffect,  FindArgu, FindVol, FindWhere;
static	short		RInstru,  RNote,  REffect,  RArgu, RVol, RWhere;
static	Boolean		FInstruActi, FNoteActi, FEffectActi, FArguActi, FVolActi;
static	Boolean		RInstruActi, RNoteActi, REffectActi, RArguActi, RVolActi;
static	Boolean		FindModify;
static	long		FindTrack;

void SetCtlName(DialogPtr theDialog, short theItem, Str255 title)
{
	Handle	aHandle;
	Rect	aRect;
	short	aType;
	
	GetDialogItem(theDialog,theItem,&aType,&aHandle,&aRect);
	SetControlTitle( (ControlHandle) aHandle, title);
}

void InitFindReplace(void)
{
	FInstruActi	= true;
	FNoteActi	= false;
	FEffectActi	= false;
	FArguActi	= false;
	FVolActi	= false;

	RInstruActi	= true;
	RNoteActi	= false;
	REffectActi	= false;
	RArguActi	= false;
	RVolActi	= false;

	FindInstru	= 1;
	FindNote	= 30;
	FindEffect	= 0;
	FindArgu	= 0;
	FindVol		= 0;

	RInstru	= 1;
	RNote	= 30;
	REffect	= 0;
	RArgu	= 0;
	RVol	= 0;
	
	FindModify = true;

	FindWhere = CurrentPat;
	FindTrack = -1;
}

static	short	DStartPos, DEndPos, DPat, DStartTrack, DEndTrack;
static	short	FStartPos, FEndPos, FStartPat, FEndPat, FStartTrack, FEndTrack;
static	short 	FPat, FPos, FTrack;

void NReplaceInt( Cmd	*myCmd, short track, short position, Boolean MakeUpdate)
{
Point	cell;

	if( RInstruActi)	myCmd->ins = RInstru;
	if( RNoteActi)		myCmd->note = RNote;
	if( REffectActi)	myCmd->cmd = REffect;
	if( RArguActi)		myCmd->arg = RArgu;
	if( RVolActi)		{ if( RVol == 0) myCmd->vol = 0xFF; else myCmd->vol = RVol;}

	if( MakeUpdate)
	{
		UPDATE_Note( position, track);
	}
	
	curMusic->hasChanged = true;
}

void ReplaceCommand(void)
{
Cmd	*myCmd;
short	track, position;

	switch( GetWRefCon( oldWindow))
	{
		default:
		case RefPartition:
			if( EditorDlog == NULL) break;
			
			myCmd = GetEditorSelectCmd( &track, &position);
			if( myCmd == NULL) break;
			
			FindInstru	= myCmd->ins;
			FindNote	= myCmd->note;
			FindEffect	= myCmd->cmd;
			FindArgu	= myCmd->arg;
			FindVol		= myCmd->vol;	if( FindVol == 0xFF) FindVol = 0;
		break;
	}
}

Boolean FindInternal( short FindType)
{
Boolean				goodInstru, goodNote, goodEffect, goodArgu, goodVol;

	if( !SetUpFind()) return false;

	switch( FindType)
	{
		case Next:
			FPat = MADDriver->Pat;
			FPos = MADDriver->PartitionReader;
			goto InBoucle;
		break;
		
		case Find:
		break;
		
		case ReplaceFind:
			ReplaceCommand();
			
			FPat = MADDriver->Pat;
			FPos = MADDriver->PartitionReader;
			goto InBoucle;
		break;
		
		case ReplaceAll:
			
		break;
	}
	
	for( FPat = FStartPat; FPat < FEndPat; FPat++)
	{
		if( FindWhere == Everywhere) FEndPos = curMusic->partition[ FPat]->header.size;

		for( FPos = FStartPos; FPos < FEndPos; FPos++)
		{
			for( FTrack = FStartTrack; FTrack< FEndTrack; FTrack++)
			{
				Cmd		*cmd;
				
				if( FindTrack != -1)
				{
					if( FindTrack != FTrack) goto InBoucle;
				}
				
				cmd = GetMADCommand( FPos, FTrack, curMusic->partition[ FPat]);
				
				if( FInstruActi)
				{
					if( cmd->ins == FindInstru) goodInstru = true;
					else goodInstru = false;
				}
				else goodInstru = true;
				
				if( FNoteActi)
				{
					if( cmd->note == FindNote) goodNote = true;
					else goodNote = false;
				}
				else goodNote = true;
				
				if( FEffectActi)
				{
					if( cmd->cmd == FindEffect) goodEffect = true;
					else goodEffect = false;
				}
				else goodEffect = true;
				
				if( FArguActi)
				{
					if( cmd->arg == FindArgu) goodArgu = true;
					else goodArgu = false;
				}
				else goodArgu = true;
				
				if( FVolActi)
				{
					if( cmd->vol == FindVol) goodVol = true;
					else goodVol = false;
				}
				else goodVol = true;
				
				if( goodInstru == true && goodNote == true && goodEffect == true && goodArgu == true && goodVol == true)
				{
					/*** WE FOUND IT !!! ***/
					
					MADDriver->PartitionReader = FPos;
					
					if(	FindType == Find ||
						FindType == Next ||
						FindType == ReplaceFind)
					{
						SetSelectionEditor( FPat, FPos, FTrack);
					//	SetSelectionClassical( FPat, FPos, FTrack);
						
						return true;
					}
					else if( FindType == ReplaceAll)
					{
						NReplaceInt( cmd, FTrack, FPos, false);
					}
					else MyDebugStr( __LINE__, __FILE__, "ReplaceInt ERROR");
					
					/***********************/
				}
				InBoucle:;
			}
		}
	}
	
	UPDATE_Pattern();
	
	FlashMenuBar( 0);
	SysBeep( 1);
	FlashMenuBar( 0);

	return false;
}
/*
void  UpdateFind(DialogPtr GetSelection)
{ 
		Rect   		tempRect, itemRect;
 		GrafPtr		SavePort;
 		Str255		tempStr, aStr;
 		short		itemType, Larg, i ,x, start, end;
 		Handle		itemHandle;
 		RgnHandle	saveClip;
 		
 		GetPort( &SavePort);
 		SetPortDialogPort( FindDlog);

		BeginUpdate( GetDialogWindow( FindDlog));
	
 		UpdtDialog( FindDlog, FindDlog->visRgn);
		
		oldFrameButton( FindDlog);
		
		EndUpdate( GetDialogWindow( FindDlog));

		SetPort( SavePort);
}
*/

void SetUpButtons(void)
{
short	i;
Str255	str;

	if( MozartDlog == NULL)
	{
		ControlSwitch( 13, FindDlog, 255);
		if( FindWhere == ClassicalSelection) FindWhere = CurrentPat;
	}

	if( EditorDlog == NULL)
	{
		ControlSwitch( 11, FindDlog, 255);
		if( FindWhere == DigitalSelection) FindWhere = CurrentPat;
	}

	for( i = 10; i <= 13; i++) TurnRadio( i, FindDlog, false);
	switch( FindWhere)
	{
		case CurrentPat:			TurnRadio( 10, FindDlog, true);		break;
		case DigitalSelection:		TurnRadio( 11, FindDlog, true);		break;
		case ClassicalSelection:	TurnRadio( 13, FindDlog, true);		break;
		case Everywhere:			TurnRadio( 12, FindDlog, true);		break;
	}

	TurnRadio( 43, FindDlog, false);
	TurnRadio( 46, FindDlog, false);
	if( FindTrack == -1) TurnRadio( 46, FindDlog, true);
	else
	{
		TurnRadio( 43, FindDlog, true);
		NumToString( FindTrack+1, str);	SetDText( FindDlog, 45, str);
	}

	if( FInstruActi) TurnRadio( 3, FindDlog, true);
	if( FNoteActi) TurnRadio( 4, FindDlog, true);
	if( FEffectActi) TurnRadio( 5, FindDlog, true);
	if( FArguActi) TurnRadio( 6, FindDlog, true);
	if( FVolActi) TurnRadio( 39, FindDlog, true);
	
	if( !FInstruActi && !FNoteActi && !FEffectActi && !FArguActi && !FVolActi && !RInstruActi && !RNoteActi && !REffectActi && !RArguActi && !RVolActi)
	{
		ControlSwitch( 1, FindDlog, 255);
		ControlSwitch( 36, FindDlog, 255);
		ControlSwitch( 38, FindDlog, 255);
		ControlSwitch( 37, FindDlog, 255);
		
	//	DisableMenuItem( EditorMenu, 10);
	//	DisableMenuItem( EditorMenu, 12);
	}
	else if( !FInstruActi && !FNoteActi && !FEffectActi && !FArguActi && !FVolActi)
	{
		ControlSwitch( 1, FindDlog, 255);
		ControlSwitch( 36, FindDlog, 255);
		ControlSwitch( 38, FindDlog, 255);
		ControlSwitch( 37, FindDlog, 0);
		
	//	DisableMenuItem( EditorMenu, 10);
	//	EnableMenuItem( EditorMenu, 12);
	}
	else if( !RInstruActi && !RNoteActi && !REffectActi && !RArguActi && !RVolActi)
	{
		ControlSwitch( 1, FindDlog, 0);
	
		ControlSwitch( 36, FindDlog, 255);
		ControlSwitch( 37, FindDlog, 255);
		ControlSwitch( 38, FindDlog, 255);
		
	//	EnableMenuItem( EditorMenu, 10);
	//	DisableMenuItem( EditorMenu, 12);
	}
	else
	{
		ControlSwitch( 1, FindDlog, 0);
		ControlSwitch( 36, FindDlog, 0);
		ControlSwitch( 37, FindDlog, 0);
		ControlSwitch( 38, FindDlog, 0);
		
	//	EnableMenuItem( EditorMenu, 10);
	//	EnableMenuItem( EditorMenu, 12);
	}
	
	if( RInstruActi) TurnRadio( 21, FindDlog, true);
	if( RNoteActi) TurnRadio( 22, FindDlog, true);
	if( REffectActi) TurnRadio( 23, FindDlog, true);
	if( RArguActi) TurnRadio( 24, FindDlog, true);
	if( RVolActi) TurnRadio( 41, FindDlog, true);
}

void CreateFind(void)
{
	long					mresult, MusicSize;
	short					whichItem, itemHit, itemType, i, x, curSelecPat;
	Str255					theStr, aStr;
	GrafPtr					myPort;
	MenuHandle				thePatternMenu;
	Handle					itemHandle;
	Point					myPt;
	Rect					itemRect;
/*
	if( FindDlog != 0L)
	{
		SelectWindow2( FindDlog);
		SetPortDialogPort( FindDlog);
		return;
	}
*/
	SaveUndo( UAllPatterns, 0, "\pUndo 'Find & Replace'");

	GetPort( &myPort);

	if( MADDriver->Reading) DoPause();
	
	GetDigitalSelection( &DStartTrack, &DStartPos, &DEndTrack, &DEndPos, NULL);
	
	FindDlog = GetNewDialog( 152,NULL, (WindowPtr) -1L);
	SetPortDialogPort( FindDlog);
	AutoPosition( FindDlog);
//	ChangeDialogFont( FindDlog);
	SetUpButtons();	
	
	SetCursor( GetQDGlobalsArrow( &qdarrow));

	/*** FIND ***/

//	NumToString( FindInstru, aStr);				// FInstrument
	NTStr( 3, FindInstru, (Ptr) aStr);	MyC2PStr( (Ptr) aStr);
	SetDText( FindDlog, 15, aStr);

	GetNoteString( FindNote, aStr);				// FNote
	SetDText( FindDlog, 17, aStr);

	OutPutHexShort( aStr, 1, FindEffect);		// FEffect
	SetDText( FindDlog, 20, aStr);

	OutPutHexShort( aStr, 2, FindArgu);			// FArgument
	SetDText( FindDlog, 18, aStr);

	OutPutHexShort( aStr, 2, FindVol);			// FVol
	SetDText( FindDlog, 40, aStr);

	/*** REPLACE ***/

//	NumToString( RInstru, aStr);				// RInstrument
	NTStr( 3, RInstru, (Ptr) aStr);	MyC2PStr( (Ptr) aStr);
	SetDText( FindDlog, 26, aStr);

	GetNoteString( RNote, aStr);				// RNote
	SetDText( FindDlog, 28, aStr);

	OutPutHexShort( aStr, 1, REffect);			// REffect
	SetDText( FindDlog, 31, aStr);

	OutPutHexShort( aStr, 2, RArgu);			// RArgument
	SetDText( FindDlog, 29, aStr);

	OutPutHexShort( aStr, 2, RVol);				// RVol
	SetDText( FindDlog, 42, aStr);

	ShowWindow( GetDialogWindow( FindDlog));
	SelectWindow( GetDialogWindow( FindDlog));

	ControlSwitch( 37, FindDlog, 255);		// Replace & Replace-Find button
	ControlSwitch( 36, FindDlog, 255);


	if( FindModify)
	{
		SetCtlName( FindDlog, 1, "\pFind");
		SetDigitalSelection( DStartTrack, DStartPos, DEndTrack, DEndPos);
		ControlSwitch( 37, FindDlog, 255);
		ControlSwitch( 36, FindDlog, 255);
	}
	else
	{
		SetCtlName( FindDlog, 1, "\pNext");
		ControlSwitch( 37, FindDlog, 0);
		ControlSwitch( 36, FindDlog, 0);
	}

	do
	{
	//	ModalDialog( MyDlgFilterDesc, &itemHit);
		MyModalDialog( FindDlog, &itemHit);

		DoItemPressFind( itemHit, FindDlog);
		
	}while( itemHit != 2);
	
	DisposeDialog( FindDlog);
	FindDlog = NULL;
	SetPort( myPort);
}

/*void CloseFind()
{
	if( FindDlog != 0L) DisposDialog( FindDlog);
	FindDlog = 0L;
}
*/

short ExtractFunctionArgu( Str255 aStr)
{
	short Oct, tt;

	UpperString( aStr, true);

	if( aStr[ 0] == 1)
	{
		if( aStr[ 1] >= 'A' && aStr[ 1] <= 'F') Oct = 10 + aStr[ 1] - 'A';
		else if( aStr[ 1] >= '0' && aStr[ 1] <= '9') Oct = aStr[ 1] - '0';
		else return -1;
	}
	else if( aStr[ 0] == 2)
	{
		if( aStr[ 1] >= 'A' && aStr[ 1] <= 'F') Oct = 10 + aStr[ 1] - 'A';
		else if( aStr[ 1] >= '0' && aStr[ 1] <= '9') Oct = aStr[ 1] - '0';
		else return -1;
		
		if( aStr[ 2] >= 'A' && aStr[ 2] <= 'F') tt = 10 + aStr[ 2] - 'A';
		else if( aStr[ 2] >= '0' && aStr[ 2] <= '9') tt = aStr[ 2] - '0';
		else return -1;
		
		Oct = Oct*16 + tt;
	}
	else return -1;
	
	return Oct;
}

Boolean SetUpFind(void)
{
	Str255	tempStr;
	
	GetDText( FindDlog, 42, tempStr);
	RVol = ExtractFunctionArgu( tempStr);
	if( RVol == -1)
	{
		SelectDialogItemText( FindDlog, 29, 0, 32767);
		Erreur( 80, -82);
		RVol = 0;
		return false;
	}
	
	GetDText (FindDlog, 40, tempStr);
	FindVol = ExtractFunctionArgu( tempStr);
	if( FindVol == -1)
	{
		SelectDialogItemText( FindDlog, 18, 0, 32767);
		Erreur( 80, -83);
		FindVol = 0;
		return false;
	}

	GetDText( FindDlog, 29, tempStr);
	RArgu = ExtractFunctionArgu( tempStr);
	if( RArgu == -1)
	{
		SelectDialogItemText( FindDlog, 29, 0, 32767);
		Erreur( 59, -82);
		RArgu = 0;
		return false;
	}
	
	GetDText (FindDlog, 18, tempStr);
	FindArgu = ExtractFunctionArgu( tempStr);
	if( FindArgu == -1)
	{
		SelectDialogItemText( FindDlog, 18, 0, 32767);
		Erreur( 59, -83);
		FindArgu = 0;
		return false;
	}
	
	if( FindWhere == Everywhere)
	{	// all patterns
		FStartPat = 0;		FEndPat = curMusic->header->numPat;
		
		// all positions
		FStartPos = 0;		FEndPos = 64;		// 
		
		// all tracks
		FStartTrack = 0;	FEndTrack = curMusic->header->numChn;
	}
	else
	{	// Current pattern
		FStartPat = MADDriver->Pat;	FEndPat = MADDriver->Pat + 1;
		
		if( FindWhere == DigitalSelection)
		{
			FStartPos 	= DStartPos;
			FEndPos 	= DEndPos;
			FStartTrack	= DStartTrack;
			FEndTrack 	= DEndTrack;
		}
		else if( FindWhere == ClassicalSelection)
		{
		//	GetClassicalSelection( &FStartPos, &FEndPos, &FStartTrack, &FEndTrack);
		}
		else if( FindWhere == CurrentPat)
		{
			FStartPos = 0;		FEndPos = curMusic->partition[ FStartPat]->header.size;
			FStartTrack = 0;	FEndTrack = curMusic->header->numChn;
		}
		else MyDebugStr( __LINE__, __FILE__, "Find ERROR");
	}
	
	return true;
}

void DoItemPressFind( short whichItem, DialogPtr whichDialog)
{
		Cell			theCell;
		long			tempLong;
		Handle			itemHandle;
		Rect			itemRect;
 		short			temp, bogus, itemType, i, *shortPtr, tempInstru, track, position;
 		long			mresult;
 		GrafPtr			SavePort;
 		Point			myPt;
 		Str255			aStr, theStr;
 		Boolean			FindModifyBefore;
 		Cmd				*myCmd;
 		
 		GetPort( &SavePort);
 		SetPortDialogPort( FindDlog);
 
	//	if( whichItem > 0) SetUpButtons();
 
 		FindModifyBefore = FindModify;
 
 		switch( whichItem)
		{
		/** FIND **/
		case 3:
		case 4:
		case 5:
		case 6:
		case 39:
			FindModify = true;
			
			InverseRadio( whichItem, FindDlog);
			
			switch( whichItem)
			{
				case 3:		FInstruActi	= !FInstruActi;		break;
				case 4:		FNoteActi	= !FNoteActi;		break;
				case 5:		FEffectActi	= !FEffectActi;		break;
				case 6:		FArguActi	= !FArguActi;		break;
				case 39:	FVolActi	= !FVolActi;		break;
			}
		break;
		/***/

		/** REPLACE **/
		case 21:
		case 22:
		case 23:
		case 24:
		case 41:
		//	FindModify = true;
			
			InverseRadio( whichItem, FindDlog);
			
			switch( whichItem)
			{
				case 21:		RInstruActi	= !RInstruActi;		break;
				case 22:		RNoteActi	= !RNoteActi;		break;
				case 23:		REffectActi	= !REffectActi;		break;
				case 24:		RArguActi	= !RArguActi;		break;
				case 41:		RVolActi	= !RVolActi;		break;
			}
		break;
		/***/

		/** WHERE **/
		case 10:
		case 11:
		case 12:
		case 13:
			FindModify = true;
			
			for( i = 10; i <= 13; i++) TurnRadio( i, FindDlog, false);
			TurnRadio( whichItem, FindDlog, true);
			
			switch( whichItem)
			{
				case 10:	FindWhere	= CurrentPat;			break;
				case 11:	FindWhere	= DigitalSelection;		break;
				case 13:	FindWhere	= ClassicalSelection;	break;
				case 12:	FindWhere	= Everywhere;			break;
			}
		break;
		/***/
		
		case 46:
		case 43:
			FindModify = true;
			
			TurnRadio( 46, FindDlog, false);
			TurnRadio( 43, FindDlog, false);
			TurnRadio( whichItem, FindDlog, true);
			
			switch( whichItem)
			{
				case 46:	FindTrack	= -1;		break;
				case 43:	GetDText( FindDlog, 45, theStr);	StringToNum( theStr, &FindTrack);	FindTrack--;	break;
			}
		break;

		case 44:
			InsertMenu( TrackView, hierMenu);
			GetDialogItem( FindDlog, whichItem, &itemType, &itemHandle, &itemRect);
			
			myPt.v = itemRect.top;	myPt.h = itemRect.left;
			LocalToGlobal( &myPt);
			
			SetItemMark( TrackView, FindTrack + 1, 0xa5);
			
			mresult = PopUpMenuSelect(	TrackView,
										myPt.v,
										myPt.h,
										FindTrack + 1);
			
			SetItemMark( TrackView, FindTrack + 1, 0);
			
			if ( HiWord(mresult ) != 0 )
			{
				FindTrack = (Byte) LoWord( mresult) - 1;
				
				NumToString( FindTrack + 1, theStr);
				SetDText( FindDlog, 45, theStr);
				
				TurnRadio( 46, FindDlog, false);
				TurnRadio( 43, FindDlog, true);

				FindModify = true;
			}
			DeleteMenu( GetMenuID( TrackView));
		break;

		case 25:
		case 14:		// Instru menu
			InsertMenuItem( InstruMenu, "\pNo Ins", 0);
		
			if( whichItem == 25) shortPtr = &RInstru;
			else shortPtr = &FindInstru;
				
			InsertMenu( InstruMenu, hierMenu);
			GetDialogItem( FindDlog, whichItem, &itemType, &itemHandle, &itemRect);
			
			myPt.v = itemRect.top;	myPt.h = itemRect.left;
			LocalToGlobal( &myPt);
			
			SetItemMark( InstruMenu, *shortPtr + 1, 0xa5);
			
			mresult = PopUpMenuSelect(	InstruMenu,
										myPt.v,
										myPt.h,
										*shortPtr + 1);		// curInstru + 1
			
			SetItemMark( InstruMenu, *shortPtr + 1, 0);		// curInstru + 1
			
			if ( HiWord(mresult ) != 0 )
			{
				*shortPtr = (Byte) LoWord( mresult) - 1;
				
			//=	NumToString( *shortPtr, aStr);
				NTStr( 3,  *shortPtr, (Ptr) aStr);	MyC2PStr( (Ptr) aStr);
				SetDText( FindDlog, whichItem + 1, aStr);
				
				if( whichItem == 25)
				{
					RInstruActi = true;
					TurnRadio( 21, FindDlog, true);
				}
				else
				{
					FInstruActi = true;
					TurnRadio( 3, FindDlog, true);
					FindModify = true;
				}
			}
			DeleteMenu( GetMenuID( InstruMenu));
			DeleteMenuItem( InstruMenu, 1);
		break;
		
		case 27:
		case 16:		// Note menu
			InsertMenuItem( NoteMenu, "\p000", NUMBER_NOTES);
			InsertMenuItem( NoteMenu, "\pOFF", NUMBER_NOTES+1);		

			if( whichItem == 27) shortPtr = &RNote;
			else shortPtr = &FindNote;
			
			tempInstru = *shortPtr;
			if( tempInstru == 0xFF) tempInstru = NUMBER_NOTES;
			if( tempInstru == 0xFE) tempInstru = NUMBER_NOTES+1;
			
			InsertMenu( NoteMenu, hierMenu );
			GetDialogItem( FindDlog, whichItem, &itemType, &itemHandle, &itemRect);
			
			myPt.v = itemRect.top;	myPt.h = itemRect.left;
			LocalToGlobal( &myPt);
			
			SetItemMark( NoteMenu, tempInstru+1, 0xa5);
			
			mresult = PopUpMenuSelect(	NoteMenu,
										myPt.v,
										myPt.h,
										tempInstru+1);
			
			SetItemMark( NoteMenu, tempInstru+1, 0);
			
			if ( HiWord(mresult ) != 0 )
			{
				*shortPtr = LoWord( mresult)-1;
				
				if( *shortPtr == NUMBER_NOTES) 		*shortPtr = 0xFF;
				if( *shortPtr == NUMBER_NOTES+1) 	*shortPtr = 0xFE;

				GetNoteString( *shortPtr, aStr);
				SetDText( FindDlog, whichItem + 1, aStr);
				
				if( whichItem == 27)
				{
					RNoteActi = true;
					TurnRadio( 22, FindDlog, true);
				}
				else
				{
					FNoteActi = true;
					TurnRadio( 4, FindDlog, true);
					FindModify = true;
				}
			}
			DeleteMenu( GetMenuID( NoteMenu));
			DeleteMenuItem( NoteMenu, NUMBER_NOTES+1);
			DeleteMenuItem( NoteMenu, NUMBER_NOTES+1);
		break;
		
		case 30:
		case 19:	// Effect menu
			if( whichItem == 30) shortPtr = &REffect;
			else shortPtr = &FindEffect;

			InsertMenu( EffectMenu, hierMenu );
			GetDialogItem( FindDlog, whichItem, &itemType, &itemHandle, &itemRect);
			
			myPt.v = itemRect.top;	myPt.h = itemRect.left;
			LocalToGlobal( &myPt);
			
			SetItemMark( EffectMenu, *shortPtr + 1, 0xa5);
			
			mresult = PopUpMenuSelect(	EffectMenu,
										myPt.v,
										myPt.h,
										*shortPtr + 1);
			
			SetItemMark( EffectMenu, *shortPtr + 1, 0);
			
			if ( HiWord(mresult ) != 0 )
			{
				*shortPtr = (Byte) LoWord( mresult) - 1;
				
				OutPutHexShort( aStr, 1, *shortPtr);
				SetDText( FindDlog, whichItem + 1, aStr);
				
				if( whichItem == 30)
				{
					REffectActi = true;
					TurnRadio( 23, FindDlog, true);
				}
				else
				{
					FEffectActi = true;
					TurnRadio( 5, FindDlog, true);
					FindModify = true;
				}
			}
			DeleteMenu( GetMenuID( EffectMenu));
		break;
		
		case 1:		// FIND
			if( FindModify == true)
			{
				if( FindInternal( Find)) FindModify = false;
				else FindModify = true;
			}
			else
			{
				if( !FindInternal( Next)) FindModify = true;
			}
		break;
		
		case 37:		// REPLACE
			if( !SetUpFind()) break;
		
			myCmd = GetEditorSelectCmd( &track, &position);
			if( myCmd == NULL) break;
			
			NReplaceInt( myCmd, track, position, true);
		break;
		
		case 36:		// REPLACE & FIND
		//	FindInternal( ReplaceFind);
		break;
		
		case 38:		// REPLACE ALL
			FindInternal( ReplaceAll);
			FindModify = true;
		break;
		}
		
		if( FindModifyBefore != FindModify)
		{
			if( FindModify)
			{
				SetCtlName( FindDlog, 1, "\pFind");
				SetDigitalSelection( DStartTrack, DStartPos, DEndTrack, DEndPos);
				ControlSwitch( 37, FindDlog, 255);
				ControlSwitch( 36, FindDlog, 255);
			}
			else
			{
				SetCtlName( FindDlog, 1, "\pNext");
				ControlSwitch( 37, FindDlog, 0);
				ControlSwitch( 36, FindDlog, 0);
			}
		}
		
	//	if( whichItem > 0) SetUpButtons();
}

void DoKeyPressFind( short theChar)
{
Boolean		GoodNote = false;
GrafPtr		savePort;
Point		myPt;
Rect		itemRect;
Handle		itemHandle;
short		itemType;

	GetPort( &savePort);
	SetPortDialogPort( FindDlog);
	
/*
	GetDialogItem( FindDlog, 18, &itemType, &itemHandle, &itemRect);
	TEKey( (char) theChar, (TEHandle) itemHandle);

	GetDialogItem( FindDlog, 29, &itemType, &itemHandle, &itemRect);
	TEKey( (char) theChar, (TEHandle) itemHandle);
*/
	SetPort( savePort);
}
