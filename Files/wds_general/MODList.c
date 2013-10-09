#include "Shuddup.h"
#include "FileUtils.h"
#include "MAD.h"
#include "RDriver.h"
#include "RDriverInt.h"
#include "PrivateList.h"
#include "GetFileIcon.h"

#define BIGMUSICLIST	404
#define MAXITEM			12000

void DrawNumberMusic();
void FileInformations( short whichItem);
void ReceiveNewFSSpec( AEDesc *dropLocation, FSSpec	*newFSSpec);
void DrawPathName();
void OpenMODListSTCf( FSSpec);
void PathNameFromDirID(long dirID, short vRefNum, StringPtr fullPathName);
void MDrawItem( Boolean selected, Rect* bounds, Str255 data, Str63 Str, FSSpec*);
Boolean PLSearch( Ptr text, short size, short id, Point *cell, PrivateList	*aL);
OSErr GereMusicListChanged();

	/******** HELP MODULE ********/
	enum
	{
		HAZ		= 32,
		HLoad	= 29,
		HDel	= 31,
		HInfo	= 27,
		HopenA	= 28,
		HPref	= 19,
		HWind	= 30,
		HList	= 2,
		HWhere	= 26
	};
#define	AHELPSIZE	9

	static	short					AHelp[ AHELPSIZE] = { HAZ, HLoad, HDel, HInfo, HopenA, HPref, HWind, HList, HWhere};

	/*****************************/


	extern	long					TickInterne;
	extern	EventRecord				theEvent;
	extern	RGBColor				theColor;
	extern	Boolean					DragManagerUse;
	extern	WindowPtr				oldWindow;
	extern	Cursor					HandCrsr, beamCrsr, pencilCrsr, CHandCrsr;
	extern	KeyMap					km;
	
			DialogPtr				MODListDlog;
	
	static	ControlHandle			InfoBut, OpenBut, LoadBut, FlipBut, DelBut, AZBut, prefBut;
	static	Boolean					canAcceptDrag;
			short					DropZone;
			Boolean					ReceivedAMusicInMusicList = false;
	static	Str255					PathNameStr;
	
	static	FSSpec					curMusicList;
	static	Boolean					changedMusicList, curMusicListFile;
	
#define		MLNONAME	"\pUntitled Music List"
	
//	static	ListSearchUPP			mySearchUPP, mySearchUPP2;	

//	static pascal Boolean myDragClickLoopMOD(void);

	/** Supporting Keyboard Navigation of list **/
	
	static	Str255					gListNavigateString;
	static	short					gTSThresh;
	static	long					gLastKeyTime;
	static	PrivateList				myList;
	static	FSSpec					*specList[ MAXITEM];
	static	unsigned char			*pathStrList[ MAXITEM];
	static	EventHandlerUPP 		myWinEvtHandler;
	
	
	/********************************************/

	void DoHelpMODList( short **items, short *lsize)
	{
		*lsize = AHELPSIZE;
		*items = AHelp;
	//	UpdateAHelpInfo( AHelp, AHELPSIZE, MODListDlog);
	}
OSErr DoShowInfo( Point	theCell);
void DoKeyPressMODList( short theChar);

short MLAddRow( short no, short pos)
{
	short i, tt;
	
	if( pos > myList.maxY) pos = myList.maxY;
	
	if( pos >= MAXITEM) return MAXITEM - 1;
	
	for( i = myList.maxY-1; i >= pos; i--)
	{
		specList[ i + no] = specList[ i];
		pathStrList[ i + no] = pathStrList[ i];
	}
	
	myList.maxY += no;
	
	for( i = pos; i < pos + no; i++)
	{
		specList[ i] = (FSSpec*) NewPtrClear( sizeof( FSSpec));
		pathStrList[ i] = (unsigned char*) -1L;
	}
	
	InvalWindowRect( GetDialogWindow( MODListDlog), &myList.rect);
	
	/**/
	
	tt = myList.maxY  - (myList.rect.bottom - myList.rect.top) / myList.HCell;	if( tt < 0) tt = 0;
	SetControlMaximum( myList.yScroll, tt);
	
	/**/
	
	changedMusicList = true;
	
	return pos;
}

void MLDelRow( short no, short pos)
{
	short i;
	
	for( i = pos; i < pos + no; i++)
	{
		if( i < myList.maxY)
		{
			MyDisposePtr( (Ptr*) &specList[ i]);
			MyDisposePtr( (Ptr*) &pathStrList[ i]);
			
			pathStrList[ i] = (unsigned char*) -1L;
			specList[ i] = (FSSpec*) -1L;
		}
	}
	
	for( i = pos; i < myList.maxY; i++)
	{
		if( i + no < MAXITEM)
		{
			specList[ i] = specList[ i + no];
			specList[ i + no] = (FSSpec*) -1L;
			
			pathStrList[ i] = pathStrList[ i + no];
			pathStrList[ i + no] = (unsigned char*) -1L;
		}
	}
	
	myList.maxY -= no;
	if( myList.maxY < 0) MyDebugStr(__LINE__, __FILE__, "");
	PLSetControl( &myList);
	PLSetSelect( myList.select.left, myList.select.top, myList.select.right, myList.select.bottom, &myList);
	
	InvalWindowRect( GetDialogWindow( MODListDlog), &myList.rect);
	
	DrawNumberMusic();
	
	changedMusicList = true;
}

void DrawMODListItem( short iD)
{
	Str255		aStr, bStr;
	Rect		tempRect;

	if( iD >= myList.maxY) return;
	if( iD < 0) return;

	if( iD+1 < 10) pStrcpy( aStr, "\p00");
	else if( iD+1 < 100) pStrcpy( aStr, "\p0");
	else pStrcpy( aStr, "\p");
	NumToString( iD+1, bStr);
	pStrcat( aStr, bStr);
	
	tempRect.left = myList.rect.left;
	tempRect.right = myList.rect.right;
	
	tempRect.top = myList.rect.top + myList.HCell* (iD - GetControlValue( myList.yScroll));
	tempRect.bottom = tempRect.top + myList.HCell;
	
	if( iD >= myList.select.top && iD <= myList.select.bottom) MDrawItem( true, &tempRect, specList[ iD]->name, aStr, specList[ iD]);
	else MDrawItem( false, &tempRect, specList[ iD]->name, aStr, specList[ iD]);
	
	/**/
	
/*	MoveTo( tempRect.left, tempRect.top);
	LineTo( tempRect.right, tempRect.top);

	MoveTo( tempRect.left, tempRect.bottom);
	LineTo( tempRect.right, tempRect.bottom);*/
}

pascal short MyOMatchNextList( Ptr cellDataPtr, Ptr searchDataPtr, short cellDataLen, short searchDataLen)
{
	if( cellDataLen != searchDataLen) return true;

	return memcmp( cellDataPtr, searchDataPtr, cellDataLen);
}

pascal short MyOMatchNextAlphabetically( Ptr cellDataPtr, Ptr searchDataPtr, short cellDataLen, short searchDataLen)
{
	if( cellDataLen > 0)
	{
		if( IdenticalText( cellDataPtr, searchDataPtr, searchDataLen, searchDataLen, NULL) == 0)
		{
			return false;
		}
	}
	return true;
}

void MyResetTypeSelection(void)
{
#define kMaxKeyThresh 120
	
	pStrcpy( gListNavigateString, "\p");
	gLastKeyTime = 0;
	gTSThresh = 2 * LMGetKeyThresh();
	
	if( gTSThresh > kMaxKeyThresh) gTSThresh = kMaxKeyThresh;
}

void MyKeySearchInList(short newChar)
{
	Cell	theCell, tCell;

	if( theEvent.when - gLastKeyTime >= gTSThresh || gListNavigateString[ 0] >= 255)
		MyResetTypeSelection();
		
	gLastKeyTime = theEvent.when;
	gListNavigateString[ 0] ++;
	gListNavigateString[ gListNavigateString[ 0]] = newChar;
	
	theCell.v = theCell.h = 0;
	
	if( PLSearch( (Ptr) (gListNavigateString + 1), gListNavigateString[ 0], 2, &theCell, &myList))
	{
		tCell.v = tCell.h = 0;
		if( PLGetSelect( &tCell, &myList))
		{
			if( tCell.v == theCell.v) return;
			else PLSetSelect( -1, -1, -1, -1, &myList);
		}
		
		PLSetSelect( theCell.h, theCell.v, theCell.h, theCell.v, &myList);
		PLAutoScroll( &myList);
	}
}

pascal	void	MyLDEF(	short lMessage, Boolean lSelect, Rect* lRect, Cell lCell, short lDataOffset, short lDataLen, ListHandle lHandle);

DragTrackingHandlerUPP	MyTrackingMODListUPP;
DragReceiveHandlerUPP	MyReceiveMODListUPP;

long OpenHeaderMOD( Ptr outPtr, Str255 name, long bytesToRead);
Boolean	ImportFile( Str255	fName, short vRefNum, long parID, OSType	theType);

void EraseMODInfo(void)
{
	Str255	StrTemp;
	Handle	iHandle;
	Rect	iRect;
	short	iType;
	Rect	caRect;

	if( MODListDlog == NULL) return;
	
	GetPortBounds( GetDialogPort( MODListDlog), &caRect);
	
	if( caRect.right != BIGMUSICLIST) return;

	pStrcpy( StrTemp, "\p-");
	SetDText( MODListDlog, 12, StrTemp);
	SetDText( MODListDlog, 13, StrTemp);
	SetDText( MODListDlog, 14, StrTemp);
	SetDText( MODListDlog, 15, StrTemp);
	SetDText( MODListDlog, 16, StrTemp);
	SetDText( MODListDlog, 17, StrTemp);
	SetDText( MODListDlog, 18, StrTemp);
	
	pStrcpy( PathNameStr, "\p-");
	DrawPathName();
}

void DoGrowMODList( DialogPtr	theDialog)
{
	long		lSizeVH;
	GrafPtr		SavePort;
	Rect		temp, cellRect, tempRect;
	short		cur, tempB, tempA, itemType;
	Handle		itemHandle;
	Point		theCell = { 0, 0}, aPt = { 0, 0};
	Rect		caRect;
	BitMap		screenBits;

	GetPort( &SavePort);
 	SetPortDialogPort( theDialog);
	
	GetPortBounds( GetDialogPort( theDialog), &caRect);
	
	GetQDGlobalsScreenBits( &screenBits);
	
	temp.left = caRect.right;
	temp.top = myList.rect.top + 155;
	temp.right = caRect.right;
	temp.bottom = screenBits.bounds.bottom;

	LocalToGlobal( &aPt);

	if( temp.bottom < temp.top) temp.bottom = temp.top;
	else if( temp.bottom > screenBits.bounds.bottom - aPt.v) temp.bottom = screenBits.bounds.bottom - aPt.v -2;

	lSizeVH = 0;
	
#if MACOS9VERSION
	temp.left++;
	temp.right++;
#endif
	
	if( theEvent.what == mouseDown) lSizeVH = GrowWindow( GetDialogWindow( theDialog), theEvent.where, &temp);
	
	if( lSizeVH != 0)
	{
		tempA = LoWord( lSizeVH);
		tempB = HiWord( lSizeVH);
	}
	else
	{
		GetPortBounds( GetDialogPort( theDialog), &caRect);
		
		tempA = caRect.right;
		tempB = caRect.bottom;
	}
	
	MySizeWindow( theDialog, tempA, tempB, true);
	
	GetPortBounds( GetDialogPort( theDialog), &caRect);
	
	temp = myList.rect;
	temp.bottom = caRect.bottom;
	temp.right = caRect.right;
	EraseRect( &temp);
	InvalWindowRect( GetDialogWindow( theDialog), &temp);
	
	myList.rect.bottom = caRect.bottom - 15;
	
	PLSetControl( &myList);
	
	if( caRect.right == BIGMUSICLIST) SetControlValue( FlipBut, 0);
	else SetControlValue( FlipBut, 1);
	SetPort( SavePort);
}

void pstrinsert(StringPtr dst, StringPtr src);
void pstrcat(StringPtr dst, StringPtr src);

OSErr PathNameFromDirIDTRUE(long dirID, short vRefNum, StringPtr fullPathName)
{
	CInfoPBRec	block;
	Str255		directoryName;
	OSErr		err;


	pStrcpy( directoryName, "\p");
	pStrcpy( fullPathName, "\p");

	block.dirInfo.ioDrParID = dirID;
	block.dirInfo.ioNamePtr = directoryName;
	do {
			block.dirInfo.ioVRefNum = vRefNum;
			block.dirInfo.ioFDirIndex = -1;
			block.dirInfo.ioDrDirID = block.dirInfo.ioDrParID;
			err = PBGetCatInfo(&block, false);
			if( fullPathName[ 0] < 200)
			{
				pstrcat(directoryName, (StringPtr)"\p:");
				pstrinsert(fullPathName, directoryName);
			}
	} while (block.dirInfo.ioDrDirID != 2 && err == noErr);
	
	if( fullPathName[ 0] > 0) fullPathName[ 0]--;
	
	return err;
}

void SaveMyMODListSTCf( FSSpec spec)
{
	short				itemType,i, theNo, myRes, vRefNum, temp;
	long				inOutBytes, Id;
	Handle				aHandle;
	Rect				itemRect;
	Point				theCell, where = { -1, -1};
	OSErr				iErr;
	Boolean				Info;
	GrafPtr				myPort;
	SFTypeList			typeList;
	Ptr					aCopy;
	FSSpec				myFSS;
	Str255				tempStr;
	short				*shortPtr;
	FInfo				fndrInfo;

#define MAXSTCSIZE	400000	// 400 Kb

	iErr = FSpDelete( &spec);
	iErr = FSpCreate( &spec, 'SNPL', 'STCf', smSystemScript);
	
	FSpCreateResFile( &spec, 'SNPL', 'STCf', smSystemScript);
	myRes = FSpOpenResFile( &spec, smSystemScript);
	UseResFile( myRes);
	
	///////////////////////////////////////////////////////
	
	aHandle = MyNewHandle( 10);
	if( aHandle == NULL)
	{
		CloseResFile( myRes);
		Erreur( 5, -678);
		return;
	}
	HLock( aHandle);
	shortPtr = (short*) *aHandle;
	
	theCell.v = 0;	theCell.h = 0;
	if( PLGetSelect( &theCell, &myList))
	{
		shortPtr[ 0] = theCell.v;
	}
	else shortPtr[ 0] = -1;
	MOT16( shortPtr);

	HUnlock( aHandle);
	
	inOutBytes = 10;
	SetHandleSize( aHandle, inOutBytes);
	
	AddResource( aHandle, 'selc', 128, "\pMusic Select");
	WriteResource( aHandle);
	DetachResource( aHandle);
	MyDisposHandle( & aHandle);
	
	
	///////////////////////////////////////////////////////
	
	
	aHandle = MyNewHandle( MAXSTCSIZE);
	if( aHandle == NULL)
	{
		CloseResFile( myRes);
		Erreur( 5, -678);
		return;
	}
	HLock( aHandle);
	aCopy = *aHandle;
	*(short*) aCopy = myList.maxY*2;
	MOT16((short*)aCopy);
	aCopy += 2;
	
	theCell.v = 0;	theCell.h = 0;
	for( theCell.v = 0 ; theCell.v < myList.maxY; theCell.v++)
	{
		myFSS = *specList[ theCell.v];
		
		iErr = FSpGetFInfo( &myFSS, &fndrInfo);
		if( iErr != 0 || (myFSS.parID == 0 && myFSS.vRefNum == 0))
		{
			pStrcpy( tempStr, pathStrList[ theCell.v]);
		}
		else iErr = PathNameFromDirIDTRUE(		myFSS.parID,
												myFSS.vRefNum,
												tempStr);
		
		pStrcpy( (unsigned char*) aCopy, tempStr);
		aCopy += tempStr[ 0] +1;

		pStrcpy( (unsigned char*) aCopy, myFSS.name);
		aCopy += myFSS.name[ 0] +1;
		
		inOutBytes = (long) aCopy - (long) *aHandle;
		if( inOutBytes > MAXSTCSIZE)
		{
			MyDebugStr( __LINE__, __FILE__, "List too large for saving");
			theCell.v = myList.maxY;
		}
		
		RollCursor();
	}
	HUnlock( aHandle);

	inOutBytes = (long) aCopy - (long) *aHandle;
	SetHandleSize( aHandle, inOutBytes);
	
	AddResource( aHandle, 'STR#', 128, "\pPP Music List");
	WriteResource( aHandle);
	DetachResource( aHandle);
	MyDisposHandle( & aHandle);
	CloseResFile( myRes);
	
	SetCursor( GetQDGlobalsArrow( &qdarrow));
}

/*void SaveMyMODList( FSSpec spec)
{
short					itemType,i, theNo, myRes, vRefNum, temp;
long					inOutBytes, Id;
Handle					aHandle;
Rect					itemRect;
Point					theCell, where = { -1, -1};
OSErr					iErr;
Boolean					Info;
GrafPtr					myPort;
SFTypeList 				typeList;
Ptr						aCopy;
FSSpec					myFSS;

	iErr = FSDelete( aName, 0);
	iErr = Create( aName, 0, 'SNPL', 'sTAT');

	CreateResFile( aName);
	myRes = FSpOpenResFile( aName);
	UseResFile( myRes);

	aHandle = MyNewHandle( 10000);			HLock( aHandle);
	aCopy = *aHandle;
	*(short*) aCopy = myList.maxY;
	aCopy += 2;
	
	theCell.v = 0;	theCell.h = 0;
	for( theCell.v = 0 ; theCell.v < myList.maxY; theCell.v++)
	{
		myFSS = *specList[ theCell.v];
	
		pStrcpy( (unsigned char*) aCopy, myFSS.name);
		aCopy += myFSS.name[ 0] +1;
		
		*(short*) aCopy = myFSS.vRefNum;
		aCopy += 2;
		*(long*) aCopy = myFSS.parID;
		aCopy += 4;
	}
	HUnlock( aHandle);

	inOutBytes = (long) aCopy - (long) *aHandle;
	SetHandleSize( aHandle, inOutBytes);

	AddResource( aHandle, 'REFo', 128, "\pvRef & Dir ID");
	WriteResource( aHandle);
	DetachResource( aHandle);
	MyDisposHandle( & aHandle);
	CloseResFile( myRes);
}*/

static long MLindex = 1;

void SaveMODList(void)
{
	short				itemType,i, theNo, myRes, vRefNum, temp;
	long				inOutBytes, Id;
	Handle				aHandle;
	Rect				itemRect;
	Point				theCell, where = { -1, -1};
	OSErr				iErr;
	Boolean				Info;
	GrafPtr				myPort;
	FSSpec				spec;
	SFTypeList			typeList;
	Ptr					aCopy;
	FSSpec				myFSS;
	Str255				str, str2;

	GetPort( &myPort);

//	NumToString( MLindex, str2);
	
//	pStrcpy( str, "\pList #");
//	pStrcat( str, str2);
	
	if( DoCustomSave( "\pSave Music List as:", curMusicList.name, 'STCf', &spec)) return;
	
//	MLindex++;
	
//	iErr = HSetVol( NULL, spec.vRefNum, spec.parID);
	
	SaveMyMODListSTCf( spec);
	
	////////
	{
		Str255	aStr;
	
		curMusicList = spec;
	
		pStrcpy( aStr, curMusicList.name);
		SetWTitle( GetDialogWindow( MODListDlog), aStr);
	
		changedMusicList = false;
	}
	////////
}

void FindName( Str255	String, Str255	Str2)
{
	short	i,x;

	for(i= String[0]; i>0; i--)
	{
		if( String[i] == ':') break;
	}
	
	Str2[0] = String[0] - i;
	
	for(x=1; x<=Str2[0]; x++)
	{
		Str2[ x] = String[i+x];
	}
}

void OpenMODListSTCf( FSSpec spec)
{
	short		itemType,i, theNo, iFileRefI, val;
	short		temp;
	Handle		aHandle;
	Rect		caRect, itemRect;
	Point		theCell;
	OSErr		iErr;
	GrafPtr		myPort;
	Ptr 		aCopy;
	Str255		Str2, aStr, aStr2;
	FSSpec		myFSS;
	WDPBRec		wdpb;
	FInfo		fndrInfo;
	
	if( GereMusicListChanged()) return;
	
	iErr = FSpGetFInfo( &spec, &fndrInfo);
	if( fndrInfo.fdType != 'STCf') return;
	
	GetPort( &myPort);
	SetPortDialogPort( MODListDlog);
	
	iFileRefI = FSpOpenResFile( &spec, fsCurPerm);
	if( ResError())
	{
		return;
	}
	
	UseResFile( iFileRefI);
	
	/*******************/
	aHandle = Get1Resource( 'STR#', 128);
	if( aHandle == NULL)
	{
		CloseResFile( iFileRefI);
		Erreur( 4, ResError());
		return;
	}
	DetachResource( aHandle);
	
	
	BackColor( whiteColor);
	
	HLock( aHandle);
	theNo = *((short*)(*aHandle));		// number of musics...
	MOT16(&theNo);
	HUnlock( aHandle);
	
	theNo /= 2;
	
	if( DropZone >= 0) val = MLAddRow( theNo, DropZone);
	else val = MLAddRow( theNo, MAXITEM);
	
	theCell.h = 0;
	for( theCell.v = val, i = 1; theCell.v < theNo + val ; theCell.v++, i += 2)
	{
		GetIndString( aStr, 128, i);
		
		if( pathStrList[ theCell.v] != (unsigned char*) -1L) DisposePtr( (Ptr) pathStrList[ theCell.v]);
		pathStrList[ theCell.v] = (unsigned char*) NewPtr( aStr[ 0] + 1);
		pStrcpy( pathStrList[ theCell.v], aStr);
		
		pStrcat( aStr, "\p:");
		
		GetIndString( myFSS.name, 128, i+1);
		
		wdpb.ioNamePtr = aStr;
		iErr = PBHSetVol( &wdpb, false);
		if( iErr == noErr) HGetVol( NULL, &myFSS.vRefNum, &myFSS.parID);
		else {myFSS.vRefNum	= 0;	myFSS.parID	= 0;}
		
		//	iErr = FSpGetFInfo( &myFSS, &fndrInfo);
		//	if( iErr) Debugger();
		
		*specList[ theCell.v] = myFSS;
		
		RollCursor();
	}
	
	DisposeHandle( aHandle);
	
	RGBBackColor( &theColor);
	/*******************/
	
	/*******************/
	aHandle = Get1Resource( 'selc', 128);
	if( aHandle)
	{
		short 	*shortPtr;
		
		DetachResource( aHandle);
		
		HLock( aHandle);
		shortPtr = ((short*)(*aHandle));		// number of musics...
		MOT16(shortPtr);
		if( shortPtr[ 0] != -1)
		{
			PLSetSelect( 0, shortPtr[ 0], 0, shortPtr[ 0], &myList);
			PLAutoScroll( &myList);
			
			theCell.h = 0;
			theCell.v = shortPtr[ 0];
			
			DoShowInfo( theCell);
		}
		HUnlock( aHandle);
		
		DisposeHandle( aHandle);
	}
	
	
	CloseResFile( iFileRefI);
	
	GetPortBounds( GetDialogPort( MODListDlog), &caRect);
	
	SetRect( &itemRect, 0, myList.rect.bottom, myList.rect.right-16, caRect.bottom);
	InvalWindowRect( GetDialogWindow( MODListDlog), &itemRect);
	
	SetCursor( GetQDGlobalsArrow( &qdarrow));
	
	////////
	{
		Str255	aStr;
		HGetVol( NULL, &curMusicList.vRefNum, &curMusicList.parID);
		FSMakeFSSpec(curMusicList.vRefNum, curMusicList.parID, spec.name, &curMusicList);
		
		
		pStrcpy( aStr, curMusicList.name);
		SetWTitle( GetDialogWindow( MODListDlog), aStr);
		
		changedMusicList = false;
		curMusicListFile = true;
	}
	////////
	SetPort( myPort);
}

/*void OpenMODList2(Str255	fName, short	vRefNum)
{
short		itemType,i, theNo, iFileRefI, val;
short		temp;
Handle		aHandle;
Rect		caRect, itemRect;
Point		theCell;
OSErr		iErr;
GrafPtr		myPort;
Ptr 		aCopy;
Str255		Str2, aStr, aStr2;
FSSpec		myFSS;

if( GereMusicListChanged()) return;

GetPort( &myPort);
SetPortDialogPort( MODListDlog);

iFileRefI = FSpOpenResFile( fName);
if( ResError())
{
	CloseResFile( iFileRefI);
	return;
}
UseResFile( iFileRefI);
if( Count1Resources( 'REFo') == 0)
{
	CloseResFile( iFileRefI);
	Erreur( 4, ResError());
	return;
}

	aHandle = GetResource( 'REFo', 128);
	DetachResource( aHandle);
	HLock( aHandle);
	
	aCopy = *aHandle;
	theNo = *(short*) aCopy;
	
	aCopy += 2;
	
	BackColor( whiteColor);
	
	if( DropZone >= 0) val = MLAddRow( theNo, DropZone);
	else val = MLAddRow( theNo, MAXITEM);
	
	theCell.h = 0;
	for( theCell.v = val; theCell.v < theNo + val ; theCell.v++)
	{
		pStrcpy( myFSS.name, (unsigned char*) aCopy);
		aCopy += myFSS.name[ 0] +1;
		myFSS.vRefNum = *(short*) aCopy;
		aCopy += 2;
		myFSS.parID = *(long*) aCopy;
		aCopy += 4;
		
		*specList[ theCell.v] = myFSS;
	}
	
	HUnlock( aHandle);	MyDisposHandle( & aHandle);
	
	RGBBackColor( &theColor);

CloseResFile( iFileRefI);

GetPortBounds( GetDialogPort( MODListDlog), &caRect);

SetRect( &itemRect, 0, myList.rect.bottom, myList.rect.right-16, caRect.bottom);
InvalWindowRect( GetDialogWindow( MODListDlog), &itemRect);

////////
{
Str255	aStr;
pStrcpy( curMusicList.name, fName);
HGetVol( NULL, &curMusicList.vRefNum, &curMusicList.parID);

pStrcpy( aStr, curMusicList.name);
SetWTitle( GetDialogWindow( MODListDlog), aStr);

changedMusicList = false;
curMusicListFile = true;
}
////////

SetPort( myPort);
}*/

Boolean PLSearch( Ptr text, short size, short mode, Point *cell, PrivateList	*aL)
{
	Ptr		bPtr;
	short	bSize, i;

	for( i = 0; i < aL->maxY; i++)
	{
		switch( mode)
		{
			case 1:
				if( !MyOMatchNextList( (Ptr) (specList[ i]->name + 1), text, (short) specList[ i]->name[ 0], size))
				{
					cell->h = 0;
					cell->v = i;
					
					return true;
				}
			break;
			
			case 2:
				if( !MyOMatchNextAlphabetically( (Ptr) (specList[ i]->name + 1), text, (short) specList[ i]->name[ 0], size))
				{
					cell->h = 0;
					cell->v = i;
					
					return true;
				}
			break;
		}
	}
	
	return false;
}

short IsAlreadyHere( Str255		aStr)
{
	short		lSize;
	Point		theCell;
	FSSpec		myFSS;
	
	theCell.v = theCell.h = 0;
	
	if( PLSearch( (Ptr) (aStr + 1), aStr[ 0], 1, &theCell, &myList))
	{
		return theCell.v;
	}
	
	return -1;
}

void NMoveMusicCell( short whichCell, short noCell)
{
	short	temp, i;
	FSSpec	*myFSS;
	Str255	myPath[ 200];
	
	if( noCell > 200) noCell = 200;
	
	myFSS = (FSSpec*) NewPtr( sizeof( FSSpec) * 100);
	
	for( i = 0; i < noCell; i++)
	{
		myFSS[ i] = *specList[ whichCell + i];
		pStrcpy( myPath[ i], pathStrList[ whichCell + i]);
	}
	
	MLDelRow( noCell, whichCell);
	
	if( whichCell < DropZone) DropZone -= noCell;
	
	DropZone = MLAddRow( noCell, DropZone);
	
	for( i = 0; i < noCell; i++)
	{
		*specList[ DropZone + i] = myFSS[ i];
		pathStrList[ DropZone + i] = (unsigned char*) NewPtr( myPath[ i][ 0] + 1);
		pStrcpy( pathStrList[ DropZone + i], myPath[ i]);
	}
	changedMusicList = true;
	
	DisposePtr( (Ptr) myFSS);
}

void MODListSelectThisMusic( Str255 str)
{
	short 		i;
	GrafPtr		SavePort;
	Point		theCell;
	
	if( MODListDlog == NULL) return;
	
	GetPort( &SavePort);
	SetPortDialogPort( MODListDlog);
	
	i = IsAlreadyHere( str);
	
	if(	i != -1)
	{
		PLSetSelect( -1, -1, -1, -1, &myList);
		
		PLSetSelect( 0, i, 0, i, &myList);
		PLAutoScroll( &myList);
		
		theCell.h = 0;
		theCell.v = i;
		
		DoShowInfo( theCell);
	}
	
	SetPort( SavePort);
}

void  AddMODList( Boolean ShowInfo, Str255 theString, short		VRef, long parID)
{ 
	short   			whichCell; 				/*	The Row that we are adding */
	Point   			cSize,theCell;  		/*	Pointer to a cell in a list  */
 	Str255				tempStr, aStr, aStr2;
 	short				i, tt, val, dataLen;
 	Ptr					stemp, mainStr;
 	Boolean				Note;
 	GrafPtr				SavePort;
 	FSSpec				myFSS;
 	Rect				itemRect;
 	
	GetPort( &SavePort);
	SetPortDialogPort( MODListDlog);
	
	theCell.v = 0;	theCell.h = 0;
	
	whichCell = IsAlreadyHere( theString);
	
	if(	whichCell != -1)
	{
		if( whichCell < 0) MyDebugStr( __LINE__, __FILE__, "AddModlist err");
		
		theCell.v = whichCell;
		theCell.h = 0;
		
		specList[ theCell.v]->vRefNum = VRef;
		specList[ theCell.v]->parID = parID;
		
		
		if( DropZone >= 0) NMoveMusicCell( theCell.v, 1);
		
		if( ShowInfo)
		{
			cSize.v = cSize.h = 0;
			PLSetSelect( -1, -1, -1, -1, &myList);
			
			if( DropZone >= 0) theCell.v = DropZone;
			else theCell.v = whichCell;
			
			PLSetSelect( theCell.h, theCell.v, theCell.h, theCell.v, &myList);
			PLAutoScroll( &myList);
			RGBBackColor( &theColor);
			
			DoShowInfo( theCell);
		}
	}
	else
	{
		BackColor( whiteColor);
		
		if( DropZone >= 0) val = MLAddRow( 1, DropZone);
		else val = MLAddRow( 1, MAXITEM);
		
		theCell.v = val;
		theCell.h = 0;
		
		pStrcpy( myFSS.name, theString);
		myFSS.vRefNum = VRef;
		myFSS.parID = parID;
		
		*specList[ theCell.v] = myFSS;
		
		PathNameFromDirIDTRUE( myFSS.parID, myFSS.vRefNum, tempStr);
		
		pathStrList[ theCell.v] = (unsigned char*) NewPtr( tempStr[ 0] + 1);
		pStrcpy( pathStrList[ theCell.v], tempStr);
		
		if( ShowInfo)
		{
			cSize.v = cSize.h = 0;
			PLSetSelect( -1, -1, -1, -1, &myList);
			
			PLSetSelect( theCell.h, theCell.v, theCell.h, theCell.v, &myList);
			PLAutoScroll( &myList);
			RGBBackColor( &theColor);
			
			DoShowInfo( theCell);
		}
		else RGBBackColor( &theColor);
		
		//	SetRect( &itemRect, 0, myList.rect.bottom, myList.rect.right-16, MODListDlog->portRect.bottom);
		//	InvalWindowRect( GetDialogWindow( &itemRect);
		
		DrawNumberMusic();
	}
	
	SetPort( SavePort);
}

void UpdateMODListInfo()
{
	GrafPtr		SavePort;
	Rect		caRect;
 	
 	if( MODListDlog == NULL) return;

 	GetPort( &SavePort);
	SetPortDialogPort( MODListDlog);
	
	GetPortBounds( GetDialogPort( MODListDlog), &caRect);
	
	InvalWindowRect( GetDialogWindow( MODListDlog), &caRect);
	
	SetPort( SavePort);
}

static long lastDrawNumber = 0;

void DrawNumberMusic()
{
	Rect   		caRect, tempRect;
	Str255		aStr, tempStr;
	RGBColor	copyCol;
	
	if( lastDrawNumber < TickCount())
	{
		GetPortBounds( GetDialogPort( MODListDlog), &caRect);
		
		SetRect(	&tempRect,
					myList.rect.left + 3,
					myList.rect.bottom + 1,
					myList.rect.right - 15,
					caRect.bottom);
		
		MoveTo( 7, myList.rect.bottom + 11);
		pStrcpy( tempStr, "\pNumber of music files : ");
		NumToString( myList.maxY, aStr);
		pStrcat( tempStr, aStr);
		
		GetBackColor( &copyCol);
		RGBBackColor( &theColor);
		TETextBox( tempStr+1, tempStr[ 0], &tempRect, teFlushLeft);
		RGBBackColor( &copyCol);
	}
}

void UpdateMODListWindow( DialogPtr	GetSelection)
{
		Rect   		caRect, tempRect, itemRect;   			/* Temporary rectangle */
 		GrafPtr		SavePort;
 		short		itemType,i;
 		Handle		itemHandle;
 		Point		Location;
 		Str255		aStr, tempStr;
 		RgnHandle	visibleRegion;
 		

 		GetPort( &SavePort);
 		SetPortDialogPort( GetSelection);
 		
 		BeginUpdate( GetDialogWindow( GetSelection));
 		
 		visibleRegion = NewRgn();
		
		GetPortVisibleRegion( GetDialogPort( GetSelection), visibleRegion);
 		
 		UpdateDialog( GetSelection, visibleRegion);
 		
 		DisposeRgn( visibleRegion);
 		
 		/*********************/
 		BackColor( whiteColor);
 		
		ForeColor( whiteColor);
 		PaintRect( &myList.rect);
		ForeColor( blackColor);
 		
 		for( i = GetControlValue( myList.yScroll); i < PLGetMaxYValue( &myList); i++)
 		{
	 		DrawMODListItem( i);
 		}
 		
		RGBBackColor( &theColor);
 		/*********************/
 		
		tempRect = myList.rect;
		
		GetPortBounds( GetDialogPort( GetSelection), &caRect);
		
		MoveTo( 0, myList.rect.top-1);
		LineTo( caRect.right, myList.rect.top-1);
		
		MoveTo( 0, myList.rect.bottom);
		LineTo( caRect.right, myList.rect.bottom);
		
		DrawNumberMusic();
		
		DrawPathName();

		DrawGrowIconP( GetSelection);
		EndUpdate( GetDialogWindow( GetSelection));

		SetPort(SavePort);
}

static	ControlHandle	selectedControl;
static	Boolean			dragMusicListFile;

Boolean IsMyTypeMODList( DragReference theDrag)
{
	short           	items, index;
	FlavorFlags     	theFlags;
	ItemReference   	theItem;
	OSErr           	result;
	long				textSize;
	HFSFlavor			myFlavor;
	FInfo				fndrInfo;
	OSType				theType;
	char				tempC[ 5];
	
	dragMusicListFile = false;
	
	GetDragItemReferenceNumber(theDrag, 1, &theItem);
    
	result = GetFlavorFlags(theDrag, theItem, flavorTypeHFS, &theFlags);
	if (result == noErr)
	{
		Boolean		targetIsFolder, wasAliased;
	
		GetFlavorDataSize( theDrag, theItem, flavorTypeHFS, &textSize);
		
		GetFlavorData( theDrag, theItem, flavorTypeHFS, &myFlavor, &textSize, 0);

		ResolveAliasFile( &myFlavor.fileSpec, true, &targetIsFolder, &wasAliased);
		
		HSetVol( NULL, myFlavor.fileSpec.vRefNum, myFlavor.fileSpec.parID);
		result = FSpGetFInfo( &myFlavor.fileSpec, &fndrInfo);
		if( result != noErr) return true;		// <- Il s'agit d'un FOLDER, pas d'un FICHIER !!!
		
		if( fndrInfo.fdType == 'sTAT' || fndrInfo.fdType == 'STCf') dragMusicListFile = true;
		
		if( OpenableFile( fndrInfo.fdType, &myFlavor.fileSpec)) return true;
		else if( MADMusicIdentifyFSp( gMADLib, tempC, &myFlavor.fileSpec) == noErr) return true;
		else return false;
    }
	
    return( false);
}

pascal OSErr MyTrackingMODList(short message, WindowPtr theWindow, void *handlerRefCon, DragReference theDrag)

{	short				result, offset, i;
	long				theTime = TickCount();
	unsigned short		count, index;
	unsigned long		flavorFlags, attributes;
	ItemReference		theItem;
	RgnHandle			theRgn;
	Point				theMouse, localMouse, theCell;
	Rect				tempRect;

	if( !mainSystemDrag) return noErr;

	if ((message != kDragTrackingEnterHandler) && (!canAcceptDrag)) return( noErr);

	SetPortWindowPort( theWindow);
	
	
	GetDragAttributes(theDrag, &attributes);
	
	switch (message) {

		case kDragTrackingEnterHandler:
			canAcceptDrag = IsMyTypeMODList( theDrag);
		break;

		case kDragTrackingEnterWindow:
			
		break;

		case kDragTrackingInWindow:
			//
			//	We receive InWindow messages as long as the mouse is in one of our windows
			//	during a drag. We draw the window highlighting and blink the insertion caret
			//	when we get these messages.
			//
			
			BackColor( whiteColor);
			
			GetDragMouse(theDrag, &theMouse, NULL);
			localMouse = theMouse;
			GlobalToLocal(&localMouse);
			
			selectedControl = NULL;
			
			if (attributes & kDragInsideSenderWindow)
			{
				Rect	contrlRect;
				
				GetControlBounds( InfoBut, &contrlRect);
			
				if( PtInRect( localMouse, &contrlRect) && selectedControl == NULL) { HiliteControl( InfoBut, kControlButtonPart);		selectedControl = InfoBut;}
				else HiliteControl( InfoBut, 0);
				
				GetControlBounds( DelBut, &contrlRect);
				
				if( PtInRect( localMouse, &contrlRect) && selectedControl == NULL)	{ HiliteControl( DelBut, kControlButtonPart);		selectedControl = DelBut;}
				else HiliteControl( DelBut, 0);
				
				GetControlBounds( OpenBut, &contrlRect);
				
				if( PtInRect( localMouse, &contrlRect) && selectedControl == NULL) { HiliteControl( OpenBut, kControlButtonPart);		selectedControl = OpenBut;}
				else HiliteControl( OpenBut, 0);
			}
			
			if( PtInRect( localMouse, &myList.rect))
			{
				DropZone = localMouse.v - myList.rect.top;
				DropZone /= myList.HCell;
				DropZone += GetControlValue( myList.yScroll);
				
				if( DropZone > myList.maxY) DropZone = myList.maxY;
				
				if( dragMusicListFile)
				{
					tempRect = myList.rect;
				}
				else
				{
					tempRect.left = myList.rect.left;
					tempRect.right = myList.rect.right;
					tempRect.top = tempRect.bottom = ((DropZone - GetControlValue( myList.yScroll)) * myList.HCell) + myList.rect.top;
					tempRect.top --;	tempRect.bottom ++;
				}
				RectRgn(theRgn = NewRgn(), &tempRect);
				
				ShowDragHilite(theDrag, theRgn, true);
				DisposeRgn(theRgn);
			}
			else
			{
				DropZone = -1;
				HideDragHilite( theDrag);
			}
			
			RGBBackColor( &theColor);
		break;

		case kDragTrackingLeaveWindow:
			BackColor( whiteColor);
			HideDragHilite( theDrag);
		//	DropZone = -1;
			RGBBackColor( &theColor);
		break;

		case kDragTrackingLeaveHandler:
		//	DropZone = -1;
		break;
	}
	

	return(noErr);
}

pascal OSErr MyReceiveMODList(WindowPtr theWindow, void* handlerRefCon, DragReference theDrag)
{
	FSSpec				myFSS;
	HFSFlavor			myFlavor;
	unsigned short		items;
	ItemReference		theItem;
	DragAttributes		attributes;
	Size				textSize;
	short				index, mouseDownModifiers, mouseUpModifiers, temp;
	Point				theCell;
	OSErr				iErr;
	GrafPtr				savePort;
	OSType				sign = 'SNPL';
	AppleEvent			aeEvent, reply;
	AEDesc				target, listElem, fileList;
			
	if( !mainSystemDrag) return dragNotAcceptedErr;
	if( !canAcceptDrag) return dragNotAcceptedErr;
	
	GetPort( &savePort);
	SetPortWindowPort( theWindow);
	
	GetDragAttributes(theDrag, &attributes);
	GetDragModifiers(theDrag, NULL, &mouseDownModifiers, &mouseUpModifiers);

	if( attributes & kDragInsideSenderWindow)
	{
		if( selectedControl == NULL)
		{
			if( DropZone >= 0)
			{
				theCell.v = 0;	theCell.h = 0;
				if( PLGetSelect( &theCell, &myList))
				{
					short i, noSelec = myList.select.bottom - myList.select.top;
					
					if( DropZone < myList.select.top || DropZone > myList.select.bottom+1)
					{
						BackColor( whiteColor);
						
						HideDragHilite(theDrag);
						
						PLSetSelect( -1, -1, -1, -1, &myList);
						
						NMoveMusicCell( theCell.v, noSelec + 1);
						
						theCell.h = 0;	theCell.v = DropZone;
						PLSetSelect( theCell.h, theCell.v, theCell.h, theCell.v + noSelec, &myList);
						
						RGBBackColor( &theColor);
					}
					DropZone = -1;
					
					return noErr;
				}
				else return (dragNotAcceptedErr);
			}
			else return (dragNotAcceptedErr);
		}
		else
		{
			HiliteControl( selectedControl, 0);
			
			if( selectedControl == InfoBut)
			{
				theCell.v = 0;	theCell.h = 0;
				if( PLGetSelect( &theCell, &myList)) FileInformations( theCell.v);
			}
			if( selectedControl == DelBut) 		DoKeyPressMODList( 8);
			if( selectedControl == OpenBut)		DoKeyPressMODList( 0x0D);
			selectedControl = NULL;
			
			DropZone = -1;
			
			return noErr;
		}
	}
	
	BackColor( whiteColor);
	HideDragHilite(theDrag);
	RGBBackColor( &theColor);

	//
	//	Un fichier en provenance du Finder
	//
	
	iErr = AECreateDesc(	typeApplSignature,
							(Ptr) &sign,
							sizeof( sign),
							&target);
	
	iErr = AECreateAppleEvent(	kCoreEventClass,
								kAEOpenDocuments,
								&target,
								kAutoGenerateReturnID,
								kAnyTransactionID,
								&aeEvent);
	
	iErr = AECreateList( nil,0,false, &fileList);
	
	CountDragItems( theDrag, &items);
	for (index = 1; index <= items; index++)
	{
		GetDragItemReferenceNumber(theDrag, index, &theItem);
		
		iErr = GetFlavorDataSize(theDrag, theItem, flavorTypeHFS, &textSize);
		if (iErr == noErr)
		{
			Boolean			targetIsFolder, wasAliased;
			
			GetFlavorData(theDrag, theItem, flavorTypeHFS, &myFlavor, &textSize, 0);
			
			iErr = ResolveAliasFile( &myFlavor.fileSpec, true, &targetIsFolder, &wasAliased);
			
			iErr = AECreateDesc( typeFSS, (Ptr) &myFlavor.fileSpec, sizeof( myFlavor.fileSpec), &listElem);
			
			iErr = AEPutDesc( &fileList, 0, &listElem);
			if( iErr) return iErr;
			
			AEDisposeDesc( &listElem);
			
			iErr = AEPutParamDesc( &aeEvent, keyDirectObject,&fileList);
			if(iErr) return iErr;
		}
		else
		{
			DropZone = -1;
			
			iErr = AEDisposeDesc( &fileList);
			
			return dragNotAcceptedErr;
		}
	}
	iErr = AEDisposeDesc( &fileList);
	
	iErr = AESend(	&aeEvent,
					&reply,
					kAENoReply,
					kAENormalPriority,
					kAEDefaultTimeout,
					NULL,
					NULL);
	if( iErr) return iErr;
	
	ReceivedAMusicInMusicList = true;
	
	SetPort( savePort);
	
	return noErr;
}

Boolean NDragMusicFile( RgnHandle myRgn, EventRecord *theEvent, Point theCell, short noCell)
{
	short				result, i;
	ItemReference		theItem;
	RgnHandle			dragRegion, tempRgn;
	Point				theLoc;
	DragReference		theDrag;
	AEDesc				dropLocation;
	DragAttributes		attributes;
	short				mouseDownModifiers, mouseUpModifiers, copyText;
	short				temp;
	HFSFlavor			myNewFile;
	FInfo				fndrInfo;
	
	if( !DragManagerUse) return false;

	//******************************************
	//******************************************

	CopyRgn( myRgn, dragRegion = NewRgn());
	SetPt( &theLoc, 0, 0);
	LocalToGlobal( &theLoc);
	OffsetRgn( dragRegion, theLoc.h, theLoc.v);
	
	NewDrag( &theDrag);
	
	for( i = 0; i < noCell; i++)
	{
		Rect dragRegionRect;
		
		FSpGetFInfo( specList[ theCell.v + i], &fndrInfo);
		
		myNewFile.fileType			=	fndrInfo.fdType;
		myNewFile.fileCreator		=	fndrInfo.fdCreator;
		myNewFile.fdFlags			=	fndrInfo.fdFlags;
		myNewFile.fileSpec			=	*specList[ theCell.v + i];
		
		AddDragItemFlavor( theDrag, 1+i, flavorTypeHFS, &myNewFile, sizeof( myNewFile), 0);
		SetDragItemBounds(theDrag, 1+i, GetRegionBounds( dragRegion, &dragRegionRect));
	}
	
	tempRgn = NewRgn();
	CopyRgn(dragRegion, tempRgn);
	InsetRgn(tempRgn, 1, 1);
	DiffRgn(dragRegion, tempRgn, dragRegion);
	DisposeRgn(tempRgn);
	
	result = TrackDrag(theDrag, theEvent, dragRegion);
	if (result != noErr && result != userCanceledErr) return(true);

	//
	//	Check to see if the drop occurred in the Finder's Trash. If the drop occurred
	//	in the Finder's Trash and a copy operation wasn't specified, delete the
	//	source selection. Note that we can continute to get the attributes, drop location
	//	modifiers, etc. of the drag until we dispose of it using DisposeDrag.
	//
	
	GetDragAttributes( theDrag, &attributes);
	if( !(attributes & kDragInsideSenderApplication))
	{
		GetDropLocation(theDrag, &dropLocation);
		
		GetDragModifiers(theDrag, NULL, &mouseDownModifiers, &mouseUpModifiers);
		copyText = (mouseDownModifiers | mouseUpModifiers) & optionKey;
		
		for( i = 0; i < noCell; i++) ReceiveNewFSSpec( &dropLocation, specList[ theCell.v + i]);
		
		if ((!copyText) && ( DropLocationIsFinderTrash( &dropLocation)))
		{
			DoKeyPressMODList( 8);
		}
		else
		{

		}
		
		AEDisposeDesc(&dropLocation);
	}

	//
	//	Dispose of the drag.
	//

	DisposeDrag(theDrag);

	DisposeRgn(dragRegion);
	
	return(true);
}

static Boolean firstCall;
void DragMODSelect()
{
	Point		aPt, theCell;
	short		dataLen, i;
	Rect		cellRect;
	RgnHandle	tempRgn, srcRgnB, dstRgn;
	FSSpec		myFSS;
	Handle		gTheSuite;
	
	if( DragManagerUse)
	{
	//	SetCursor( &CHandCrsr);
		if( WaitMouseMoved( theEvent.where))
		{
			/*****************************/
			
			dstRgn = NewRgn();
			
			for( i = 0; i < myList.select.bottom - myList.select.top + 1; i++)
			{
				tempRgn = NewRgn();
				
				PLGetSelectRect( &cellRect, &myList);
				cellRect.top += myList.HCell * i;
				
				cellRect.left 	= cellRect.left + 6;
				cellRect.top 	= cellRect.top + 2;
				cellRect.bottom	= cellRect.top + 16;
				cellRect.right 	= cellRect.left + 16;
				
				myFSS = *specList[ myList.select.top + i];
				
				IconRef	iconref;
				SInt16	label;
				OSErr	iErr;
				
				iErr = GetIconRefFromFile( &myFSS, &iconref, &label);
				
				if( iErr == noErr)
				{
					IconRefToRgn( tempRgn, &cellRect, 0, kIconServicesNormalUsageFlag, iconref);
					ReleaseIconRef( iconref);
				}
				else
				{
					iErr=GetIconRef( kOnSystemDisk, 'SNPL', 'MADK', &iconref);
					if( iErr == noErr)
					{
						IconRefToRgn( tempRgn, &cellRect, 0, kIconServicesNormalUsageFlag, iconref);
						ReleaseIconRef( iconref);
					}
				}
				
				PLGetSelectRect( &cellRect, &myList);
				cellRect.top += myList.HCell * i;
				
				cellRect.left = cellRect.left + 23;
				cellRect.top = cellRect.top + 4;
				cellRect.right = cellRect.left + StringWidth( myFSS.name) + 2;
				cellRect.bottom = cellRect.top + 13;
				
				srcRgnB = NewRgn();
				OpenRgn();
				FrameRect( &cellRect);
				CloseRgn( srcRgnB);
				
				UnionRgn( tempRgn, srcRgnB, tempRgn);
				UnionRgn( tempRgn, dstRgn, dstRgn);
				DisposeRgn( srcRgnB);
				DisposeRgn( tempRgn);
			}
			/*****************************/
			
			theCell.h = 0;
			theCell.v = myList.select.top;
			
			NDragMusicFile( dstRgn, &theEvent, theCell, myList.select.bottom - myList.select.top + 1);
			
			DisposeRgn( dstRgn);
		}
	}
}

void CreateMODListWindow(void)
{
	Rect		caRect, itemRect, tempRect, dataBounds;
	Handle		itemHandle;
	short		itemType, itemHit, temp;
	short		NoDigi;
	Point		cSize;
	FontInfo	ThisFontInfo;
	Str255		String;
	
	curMusicListFile = false;
	pStrcpy( curMusicList.name, MLNONAME);
	
//	mySearchUPP = NewListSearchProc( MyOMatchNextAlphabetically);
//	mySearchUPP2 = NewListSearchProc( MyOMatchNextList);
	
	MODListDlog = GetNewDialog( 142, NULL, (WindowPtr) ToolsDlog);
	SetWindEtat( GetDialogWindow(MODListDlog));

/*	#if MACOS9VERSION
	#else
	{
	EventHandlerRef	ref;
	
	myWinEvtHandler = NewEventHandlerUPP( CarbonWindowEventHandler);
    InstallWindowEventHandler( GetDialogWindow(MODListDlog), myWinEvtHandler, 3, CarbonWinlist, 0, &ref);
	}
	#endif*/

	GetPortBounds( GetDialogPort( MODListDlog), &caRect);
	
	MySizeWindow( MODListDlog, 200, caRect.bottom, true);

	SetPortDialogPort( MODListDlog);
	TextFont( kFontIDGeneva);
	TextSize( 9);
	
	DropZone = -1;
	
	/** PrivateList **/

	SetRect( &myList.select, -1, -1, -1, -1);
	myList.HCell	= 20;
	myList.xScroll	= NULL;
	SetRect( &itemRect, 0, 0, 30, 16);
	myList.yScroll = NewControl( 	GetDialogWindow( MODListDlog),
									&itemRect,
									"\p",
									true,
									0,
									0,
									0,
									gScrollBarID,
									0);
	
	GetDialogItem( MODListDlog , 2, &itemType, &itemHandle, &myList.rect);
	
	GetPortBounds( GetDialogPort( MODListDlog), &caRect);
	
	myList.rect.bottom = caRect.bottom - 16;
	myList.rect.right -= 15;
	myList.LCell = myList.rect.right - myList.rect.left;
	myList.type = dragItem + onlyOne;
	myList.aDia = MODListDlog;
	myList.maxX	= 1;
	
	pStrcpy( PathNameStr, "\p-");

	/** Small buttons **/

/*	GetDialogItem( MODListDlog , 32, &itemType, &itemHandle, &itemRect);
//	//itemRect.right = itemRect.left;
	AZBut = NewControl( 	GetDialogWindow( MODListDlog),
							&itemRect,
							"\p",
							true,
							0,		//icl8 id - AZ List
							1,
							171,
							33,
							0);*/
	GetDialogItemAsControl( MODListDlog, 32, &AZBut);
//	SetControlAction( AZBut, );

	GetDialogItemAsControl( MODListDlog, 31, &DelBut);
//	GetDialogItem( MODListDlog , 31, &itemType, &itemHandle, &itemRect);
//	//itemRect.right = itemRect.left;
/*	DelBut = NewControl( 	GetDialogWindow( MODListDlog),
							&itemRect,
							"\p",
							true,
							0,		//icl8 id - Record
							1,
							150,
							33,
							0);*/
	GetDialogItemAsControl( MODListDlog, 27, &InfoBut);
/*	GetDialogItem( MODListDlog , 27, &itemType, &itemHandle, &itemRect);
//	itemRect.right = itemRect.left + 32;
	InfoBut = NewControl( 	GetDialogWindow( MODListDlog),
							&itemRect,
							"\p",
							true,
							0,		//icl8 id - Record
							1,
							149,
							33,
							0);*/
	GetDialogItemAsControl( MODListDlog, 28, &OpenBut);
/*	GetDialogItem( MODListDlog , 28, &itemType, &itemHandle, &itemRect);
//	itemRect.right = itemRect.left + 32;
	OpenBut = NewControl( 	GetDialogWindow( MODListDlog),
							&itemRect,
							"\p",
							true,
							0,		//icl8 id - Record
							1,
							159,
							33,
							0);*/
	GetDialogItemAsControl( MODListDlog, 29, &LoadBut );
/*	GetDialogItem( MODListDlog , 29, &itemType, &itemHandle, &itemRect);
//	itemRect.right = itemRect.left + 32;
	LoadBut = NewControl( 	GetDialogWindow( MODListDlog),
							&itemRect,
							"\p",
							true,
							0,		//icl8 id - Record
							1,
							152,
							33,
							0);*/
	GetDialogItemAsControl( MODListDlog, 19, &prefBut);
/*	GetDialogItem( MODListDlog , 19, &itemType, &itemHandle, &itemRect);
//	itemRect.right = itemRect.left + 32;
	prefBut = NewControl( 	GetDialogWindow( MODListDlog),
							&itemRect,
							"\p",
							true,
							0,		//icl8 id - Record
							1,
							174,
							33,
							0);*/
	GetDialogItemAsControl( MODListDlog, 30, &FlipBut);
/*	GetDialogItem( MODListDlog , 30, &itemType, &itemHandle, &itemRect);
//	itemRect.right = itemRect.left + 32;
	FlipBut = NewControl( 	GetDialogWindow( MODListDlog),
							&itemRect,
							"\p",
							true,
							0,		//icl8 id - Flip
							1,
							161,
							33,
							0);*/
		
	HiliteControl( InfoBut, 255);
	HiliteControl( DelBut, 255);
	HiliteControl( OpenBut, 255);
	
	GetPortBounds( GetDialogPort( MODListDlog), &caRect);
	
	SetMaxWindow( caRect.right + 3, 0, MODListDlog);

	theEvent.what = 0;
	DoGrowMODList( MODListDlog);
	
	if( DragManagerUse)
	{
		MyTrackingMODListUPP = NewDragTrackingHandlerUPP( MyTrackingMODList);
		MyReceiveMODListUPP = NewDragReceiveHandlerUPP( MyReceiveMODList);
		
		InstallTrackingHandler( (DragTrackingHandlerUPP) MyTrackingMODListUPP, GetDialogWindow( MODListDlog), (void *) NULL);
		InstallReceiveHandler( (DragReceiveHandlerUPP) MyReceiveMODListUPP, GetDialogWindow( MODListDlog), (void *) NULL);
	}
	
	{
		short i;
		for( i = 0; i < MAXITEM;i++)
		{
			specList[ i] = (FSSpec*) -1L;
			pathStrList[ i] = (unsigned char*) -1L;
		}
	}
}

void CloseMODListWindow(void)
{
	HideWindow( GetDialogWindow( MODListDlog));
	
	SetItemMark( FileMenu, 9, noMark);
}

extern 	EventRecord				theEvent;
extern	DialogPtr				dlogptr;

void PathNameFromDirID(long dirID, short vRefNum, StringPtr fullPathName);

void DrawPathName(void)
{
RgnHandle	saveClipRgn;
Rect		caRect, itemRect, destRect;
Handle		itemHandle, gTheSuite;
short		itemType;
TEHandle	hTE;

// 	saveClipRgn = NewRgn();
// 	GetClip( saveClipRgn);
 	
 	GetDialogItem( MODListDlog, 26, &itemType, &itemHandle, &itemRect);
 	
 	GetPortBounds( GetDialogPort( MODListDlog), &caRect);
 	
 	itemRect.bottom = myList.rect.bottom;
 	itemRect.right = caRect.right;
 	
// 	ClipRect( &itemRect);

// 	GetDialogItem( MODListDlog, 26, &itemType, &itemHandle, &itemRect);
	//TETextBox( PathNameStr + 1, PathNameStr[ 0], &itemRect, teJustLeft);
	
	destRect = itemRect;
	destRect.bottom = 1000;
	
	hTE = TENew( &destRect, &itemRect);
	
	TESetText( PathNameStr+1, PathNameStr[ 0], hTE);
	
	EraseRect( &itemRect);
	TEUpdate( &itemRect, hTE);
	
	TEDispose( hTE);
	
//	SetClip( saveClipRgn);
// 	DisposeRgn( saveClipRgn);
}

void ShowMusicInfo( Str255 name, short VRef, long parID, short id)
{
	MADSpec			*theMAD;
	Ptr				generalFile;
	FInfo			fndrInfo;
	short			i, QTRes;
	long			fileSize, temp;
	GrafPtr			savePort;
	Str255			StrTemp;
	OSErr			iErr;
	PPInfoRec		info;
	FSSpec			fss;
	char			tempC[ 5];
	Rect			caRect;

	GetPortBounds( GetDialogPort( MODListDlog), &caRect);
	
 	if( caRect.right == BIGMUSICLIST)
	{
		FSMakeFSSpec(VRef, parID, name, &fss);
	
	//	iErr = HSetVol( NULL, VRef, parID);
		iErr = FSpGetFInfo( &fss, &fndrInfo);
		if( iErr == fnfErr || iErr == fLckdErr)
		{
			WDPBRec		wdpb;
			
			wdpb.ioNamePtr = pathStrList[ id];
			iErr = PBHSetVol( &wdpb, false);
			if( iErr == noErr) HGetVol( NULL, &VRef, &parID);
			else {VRef	= 0;	parID	= 0;}
		}
		
		FSMakeFSSpec(VRef, parID, name, &fss);
		iErr = HSetVol( NULL, VRef, parID);
		iErr = FSpGetFInfo( &fss, &fndrInfo);
		if( iErr)
		{
			fndrInfo.fdType = '-   ';
		}
		
		fileSize = 0;
		iErr = FSpOpenDF( &fss, fsCurPerm, &QTRes);
		if( iErr == noErr)
		{
			iErr = GetEOF( QTRes, &fileSize);
			FSCloseFork( QTRes);
			
			if( fileSize == 0)
			{
				iErr = FSpOpenRF( &fss, fsCurPerm, &QTRes);
				if( iErr == noErr)
				{
					iErr = GetEOF( QTRes, &fileSize);
					FSCloseFork( QTRes);
				}
			}
		}
		fileSize /= 1024;
		
		OSType2Ptr( fndrInfo.fdType, tempC);
	 	if( OpenableFile( fndrInfo.fdType, &fss) == false && MADMusicIdentifyFSp( gMADLib, tempC, &fss) != noErr)
	 	{
	 		EraseMODInfo();
	 		
	 		pStrcpy( PathNameStr, pathStrList[ id]);
	 		DrawPathName();
	 		SetDText( MODListDlog, 12, name);
	 		
	 		if( fileSize > 1000)
			{
				sprintf( (Ptr) StrTemp, "%.2f", (double) fileSize / (double) 1024.);
				MyC2PStr( (Ptr) StrTemp);
				pStrcat( StrTemp, "\p MiB");
			}
			else
			{
				NumToString( fileSize, StrTemp);
				pStrcat( StrTemp, "\p kiB");
				
				if( fileSize == 0) pStrcpy( StrTemp, "\p-");
			}
			
			SetDText( MODListDlog, 18, StrTemp);
			
			StrTemp[ 0] = 4;
			((long*) (StrTemp + 1 ))[ 0] = fndrInfo.fdType;
			SetDText( MODListDlog, 15, StrTemp);
			
	 		return;
	 	}
	 	
	 	GetPort( &savePort);
	 	SetPortDialogPort( MODListDlog);
		
		fndrInfo.fdType = Ptr2OSType( tempC);
		
		if( fileSize > 1000)
		{
			sprintf( (Ptr) StrTemp, "%.2f", (double) fileSize / (double) 1024.);
			MyC2PStr( (Ptr) StrTemp);
			pStrcat( StrTemp, "\p MiB");
		}
		else
		{
			NumToString( fileSize, StrTemp);
			pStrcat( StrTemp, "\p kiB");
		}
		SetDText( MODListDlog, 18, StrTemp);
		
		switch( fndrInfo.fdType)
		{
			default:
				MyP2CStr( name);
				iErr = HSetVol( NULL, VRef, parID);
				iErr = PPInfoFile( gMADLib, tempC, (Ptr) name, &info);
				MyC2PStr( (Ptr) name);
				
				if( iErr == noErr)
				{
					MyC2PStr( info.internalFileName);
					SetDText( MODListDlog, 13, (unsigned char*) info.internalFileName);
					MyP2CStr( (unsigned char*) info.internalFileName);
					
					NumToString( info.totalPatterns, StrTemp);
					SetDText( MODListDlog, 14, StrTemp);
					
					MyC2PStr( info.formatDescription);
					SetDText( MODListDlog, 16, (unsigned char*) info.formatDescription);
					MyP2CStr( (unsigned char*) info.formatDescription);
					
					NumToString( info.totalInstruments, StrTemp);
					SetDText( MODListDlog, 17, StrTemp);
					
					StrTemp[ 0] = 4;
					((UInt32*) (StrTemp + 1 ))[ 0] = EndianU32_NtoB(info.signature);
					SetDText( MODListDlog, 15, StrTemp);
				}
				else
				{
					StrTemp[ 0] = 4;
					((long*) (StrTemp + 1 ))[ 0] = fndrInfo.fdType;
					SetDText( MODListDlog, 15, StrTemp);
					
					SetDText( MODListDlog, 16, "\pQuicktime Sound");
					MyP2CStr( (unsigned char*) info.formatDescription);
				}
			break;
		}
		
		PathNameFromDirID(		parID,
								VRef,
								PathNameStr);
		DrawPathName();
	
		SetDText( MODListDlog, 12, name);
				
		SetPort( savePort);
	}
}

OSErr DoShowInfo( Point	theCell)
{
Str255			StrTemp;
long			lTemp;
short			temp,i,dataLen;
OSErr			iErr;
FSSpec			myFSS;
Rect			caRect;
GrafPtr		SavePort;

	if( MODListDlog == NULL) return noErr;
			
	HiliteControl( DelBut, 0);
	HiliteControl( InfoBut, 0);
	HiliteControl( OpenBut, 0);
	
	GetPortBounds( GetDialogPort( MODListDlog), &caRect);
	
	if( caRect.right != BIGMUSICLIST) return noErr;
	
	GetPort( &SavePort);
 	SetPortDialogPort( MODListDlog);
	
	pStrcpy( StrTemp, "\p-");
	SetDText( MODListDlog, 12, StrTemp);
	SetDText( MODListDlog, 13, StrTemp);
	SetDText( MODListDlog, 14, StrTemp);
	SetDText( MODListDlog, 15, StrTemp);
	SetDText( MODListDlog, 16, StrTemp);
	SetDText( MODListDlog, 17, StrTemp);
	SetDText( MODListDlog, 18, StrTemp);
	pStrcpy( PathNameStr, "\p-");
	DrawPathName();
		
	myFSS = *specList[ theCell.v];
	ShowMusicInfo( myFSS.name, myFSS.vRefNum, myFSS.parID, theCell.v);
	
	SetPort( SavePort);
		
	return noErr;
}

static long lastWhen;

Boolean OpenFirst2( short pos)
{
	FSSpec		myFSS;
	Point		tCell;
	
	if( myList.maxY > pos && pos >= 0)
	{
		myFSS = *specList[ pos];
		
		tCell.v = tCell.h = 0;
		if( PLGetSelect( &tCell, &myList))
		{
			if( tCell.v != pos)
			{
				PLSetSelect( 0, pos, 0, pos, &myList);
				PLAutoScroll( &myList);
			}
		}
		else
		{
			PLSetSelect( 0, pos, 0, pos, &myList);
			PLAutoScroll( &myList);
		}
		
		return ImportFile( myFSS.name, myFSS.vRefNum, myFSS.parID, 0);
	}
	
	return false;
}

void ChangeTracksNo( short NewValue)
{
	Boolean IsPlaying = MADDriver->Reading;
	MADDriver->Reading = false;
	
	// Update au niveau du fichier
	SetUpPartition( NewValue);

	// Update au niveau du driver
	MADDriver->DriverSettings.numChn = curMusic->header->numChn;
	MADChangeTracks( MADDriver, MADDriver->DriverSettings.numChn);
	SetUpTracksMenu();
	
	UPDATE_Total();
	
	SetCurrentMOD( curMusic->musicFileName);
	
	MADDriver->Reading = IsPlaying;

}

void ShowCopyrightNote()
{
	long		Result, mresult;
	short		PourCent, itemHit = 0,i, itemType;
	Rect		itemRect;
	Handle		itemHandle;
	DialogPtr	TheDia;
	Str255		theStr;
	GrafPtr		myPort;
	Point		theCell, myPt;
	long		oldValue, NewValue, curSelec;
	Boolean		IsPlaying;
	
	if( curMusic->header->showCopyright == false) return;
	
	GetPort( &myPort);

	TheDia = GetNewDialog( 177,NULL, (WindowPtr) -1L);
	SetPortDialogPort( TheDia);
//	AutoPosition( TheDia);
	
	goto updateCopy;
	
	do
	{
		MyModalDialog( TheDia, &itemHit);
		
		switch( itemHit)
		{
			case -updateEvt:
				updateCopy:
				GetDialogItem( TheDia, 2, &itemType, &itemHandle, &itemRect);
				BackColor( whiteColor);
				TETextBox( (curMusic->header)->infos, strlen( (curMusic->header)->infos), &itemRect, teJustCenter);
				RGBBackColor( &theColor);
				
				InsetRect( &itemRect, -2, -2);
				ForeColor( redColor);
				PenSize( 2, 2);
				FrameRoundRect( &itemRect, 4, 4);
				PenSize( 1, 1);
				ForeColor( blackColor);
			break;
		}
	
	}while( itemHit != 1);
	
	DisposeDialog( TheDia);
	SetPort( myPort);
}

void DoChangeMODName(void)
{
	long		Result, mresult;
	short		PourCent, itemHit,i, itemType;
	Rect		itemRect;
	Handle		itemHandle;
	DialogPtr	TheDia;
	Str255		theStr;
	GrafPtr		myPort;
	Point		theCell, myPt;
	long		oldValue, NewValue, NewValue2, curSelec;
	Boolean		IsPlaying;
	
	GetPort( &myPort);

	TheDia = GetNewDialog( 148,NULL, (WindowPtr) -1L);
	SetPortDialogPort( TheDia);
	AutoPosition( TheDia);
	ChangeDialogFont( TheDia);
	
	curMusic->header->name[31] = '\0';
	SetDText( TheDia, 3, (unsigned char*) MyC2PStr( (curMusic->header)->name));
	MyP2CStr( (unsigned char*) (curMusic->header)->name);
	
	SelectDialogItemText( TheDia, 3, 0, 32767);
	
	SetDText( TheDia, 10, (unsigned char*) MyC2PStr( (curMusic->header)->infos));
	MyP2CStr( (unsigned char*) (curMusic->header)->infos);
	
	NumToString( curMusic->header->tempo, theStr);		SetDText( TheDia, 7, theStr);
	NumToString( curMusic->header->speed, theStr);		SetDText( TheDia, 8, theStr);
	NumToString( curMusic->header->numChn, theStr);	SetDText( TheDia, 14, theStr);
	oldValue = NewValue = curMusic->header->numChn;
	
	if( curMusic->header->showCopyright) TurnRadio( 15, TheDia, true);
	else TurnRadio( 15, TheDia, false);
	
/*	if( curMusic->header->MODMode) TurnRadio( 18, TheDia, true);
	else TurnRadio( 18, TheDia, false);*/
	
	if( curMusic->header->XMLinear) TurnRadio( 19, TheDia, true);
	else TurnRadio( 19, TheDia, false);
	
	TurnRadio( 18, TheDia, curMusic->header->MODMode);
	TurnRadio( 19, TheDia, curMusic->header->XMLinear);
	//ControlSwitch( 19, TheDia, 255);
	TurnRadio( 20, TheDia, curMusic->header->MultiChan);
	//ControlSwitch( 20, TheDia, 255);
	
	NumToString( curMusic->header->MultiChanNo, theStr);
	SetDText( TheDia, 22, theStr);
	NewValue2 = curMusic->header->MultiChanNo;
	
	do
	{
		//ModalDialog( MyDlgFilterDesc, &itemHit);
		MyModalDialog( TheDia, &itemHit);
		
	//	SetPortDialogPort( TheDia);
		
	//	ChangeDialogFont( TheDia);
		
		switch( itemHit)
		{
			case 21:
				InsertMenu( TracksNumberMenu, hierMenu);
				GetDialogItem( TheDia, itemHit, &itemType, &itemHandle, &itemRect);
				
				curSelec = GetTracksMenuPos( curMusic->header->MultiChanNo);
				
				myPt.v = itemRect.top;	myPt.h = itemRect.left;
				LocalToGlobal( &myPt);
				
				SetItemMark( TracksNumberMenu, curSelec, 0xa5);
				
				mresult = PopUpMenuSelect(	TracksNumberMenu,
											myPt.v,
											myPt.h,
											curSelec);
				
				SetItemMark( TracksNumberMenu, curSelec, 0);
				
				if ( HiWord(mresult ) != 0 )
				{
					NewValue2 = GetTracksMenuVal( LoWord( mresult));
				}
				DeleteMenu( GetMenuID( TracksNumberMenu));
				
				NumToString( NewValue2, theStr);
				SetDText( TheDia, 22, theStr);
			break;
			
			case 20:
				InverseRadio( 20, TheDia);
			break;
			
			case 18:
				InverseRadio( 18, TheDia);
			break;
			
			case 19:
				InverseRadio( 19, TheDia);
			break;
		
			case 16:
				InsertMenu( TracksNumberMenu, hierMenu);
				GetDialogItem( TheDia, itemHit, &itemType, &itemHandle, &itemRect);
				
				curSelec = GetTracksMenuPos( curMusic->header->numChn);
				
				myPt.v = itemRect.top;	myPt.h = itemRect.left;
				LocalToGlobal( &myPt);
				
				SetItemMark( TracksNumberMenu, curSelec, 0xa5);
				
				mresult = PopUpMenuSelect(	TracksNumberMenu,
											myPt.v,
											myPt.h,
											curSelec);
				
				SetItemMark( TracksNumberMenu, curSelec, 0);
				
				if ( HiWord(mresult ) != 0 )
				{
					NewValue = GetTracksMenuVal( LoWord( mresult));
				}
				DeleteMenu( GetMenuID( TracksNumberMenu));
				
				NumToString( NewValue, theStr);
				SetDText( TheDia, 14, theStr);
			break;
			
			case 15:
				InverseRadio( 15, TheDia);
			break;
		}
	
	}while( itemHit != 1 && itemHit != 2);


	if( itemHit == 1)
	{
		for(i=0; i<32; i++) theStr[i] = '\0';
		GetDText( TheDia, 3, theStr);
		theStr[0] = 31;		MyP2CStr( theStr);
		strcpy( (Ptr) (curMusic->header)->name, (Ptr) theStr);
		
		for(i=0; i<256; i++) theStr[ i] = '\0';
		GetDText( TheDia, 10, theStr);
		theStr[0] = INFOSSIZE;	MyP2CStr( theStr);
		strcpy( (Ptr) (curMusic->header)->infos, (Ptr) theStr);
		
		GetDText( TheDia, 7, theStr);
		StringToNum( theStr, &Result);
		if( Result > 0 && Result < 500) curMusic->header->tempo = Result;
		
		GetDText( TheDia, 8, theStr);
		StringToNum( theStr, &Result);
		if( Result > 0 && Result < 15) curMusic->header->speed = Result;
		
		GetDialogItem(TheDia, 15, &itemType, &itemHandle, &itemRect);
		if( GetControlValue( (ControlHandle) itemHandle)) curMusic->header->showCopyright = true;
		else curMusic->header->showCopyright = false;
		
		GetDialogItem(TheDia, 18, &itemType, &itemHandle, &itemRect);
		if( GetControlValue( (ControlHandle) itemHandle)) curMusic->header->MODMode = true;
		else curMusic->header->MODMode = false;
		
		GetDialogItem(TheDia, 20, &itemType, &itemHandle, &itemRect);
		if( GetControlValue( (ControlHandle) itemHandle)) curMusic->header->MultiChan = true;
		else curMusic->header->MultiChan = false;
		
		GetDialogItem(TheDia, 19, &itemType, &itemHandle, &itemRect);
		if( GetControlValue( (ControlHandle) itemHandle)) curMusic->header->XMLinear = true;
		else curMusic->header->XMLinear = false;
		
		if( curMusic->header->MODMode) MADDriver->MODMode = true;
		else MADDriver->MODMode = false;
		
		if( curMusic->header->XMLinear) MADDriver->XMLinear = true;
		else MADDriver->XMLinear = false;
		
		curMusic->header->MultiChanNo = NewValue2;
		MADDriver->MultiChanNo = curMusic->header->MultiChanNo;
		MADCleanDriver( MADDriver);
		
		curMusic->hasChanged = true;
		
		///--------------
		/// Tracks UPDATE
		
		if( oldValue != NewValue)
		{
			ChangeTracksNo( NewValue);
		}
		/// End Tracks UPDATE
		///------------------
	}
	
	DisposeDialog( TheDia);
	SetPort( myPort);
}

static short 	FileInfoID;
static Str255	FileInfoName;

void UpdateFileInformations( DialogPtr theDia)
{
	GrafPtr	savePort;
	short	itemType;
	Rect	itemRect;
	Handle	itemHandle, gTheSuite;
	Str255	aStr, bStr;
	IconRef	iconref;
	SInt16	label;
	OSErr	iErr;
		
	GetPort( &savePort);
	SetPortDialogPort( theDia);
	
	BeginUpdate( GetDialogWindow( theDia));
	
	DrawDialog( theDia);
	oldFrameButton( theDia);
		
	GetDialogItem( theDia, 11, &itemType, &itemHandle, &itemRect);
		
	EraseRect( &itemRect);
		
	iErr = GetIconRefFromFile( specList[ FileInfoID], &iconref, &label);
		
	if( iErr == noErr)
	{
		PlotIconRef( &itemRect, kAlignNone, kTransformNone, kIconServicesNormalUsageFlag, iconref);
		ReleaseIconRef( iconref);
	}
	else
	{
		iErr=GetIconRef( kOnSystemDisk, 'SNPL', 'MADK', &iconref);
		if( iErr == noErr)
		{
			PlotIconRef( &itemRect, kAlignNone, kTransformNone, kIconServicesNormalUsageFlag, iconref);
			ReleaseIconRef( iconref);
		}
	}
		
	GetDialogItem( theDia, 12, &itemType, &itemHandle, &itemRect);
	TETextBox( FileInfoName+1, FileInfoName[ 0], &itemRect, teCenter);
		
	EndUpdate( GetDialogWindow( theDia));
	SetPort( savePort);
}

short FindTYPE( Str255	aStr, MenuHandle	menu)
{
	short	i;
	Str255	bStr;
	
	for( i = 1; i <= CountMenuItems( menu); i++)
	{
		GetMenuItemText( menu, i, bStr);
		
		if( *((OSType*) (aStr+1)) == *((OSType*) (bStr+1))) return i;
	}
	
	return 1;
}

OSType GetTYPE( short item, MenuHandle	menu)
{
	Str255	bStr;

	GetMenuItemText( menu, item, bStr);
	
	return *((OSType*) (bStr + 1));
}

void FileInfoIcon( DialogPtr	theDia)
{
	short	itemType;
	Rect	itemRect;
	Handle	itemHandle;

	GetDialogItem( theDia, 11, &itemType, &itemHandle, &itemRect);
	InvalWindowRect( GetDialogWindow( theDia), &itemRect);
	
	GetDialogItem( theDia, 12, &itemType, &itemHandle, &itemRect);
	InvalWindowRect( GetDialogWindow( theDia), &itemRect);
}

void FileInformations( short whichItem)
{
	long				Result;
	short				dataLen, itemType, itemHit,i;
	DialogPtr			TheDia;
	Str255				theStr, aStr, aStr2;
	GrafPtr				myPort;
	Point				theCell, myPt;
	FSSpec				myFSS;
	Rect				caRect, itemRect;
	Handle				itemHandle;
	MenuHandle			tempMenu;
	FInfo				fndrInfo, copy;
	OSErr				iErr;
	char				tempC[ 5];
	
	myFSS = *specList[ whichItem];
	iErr = FSpGetFInfo( &myFSS, &fndrInfo);
	if( iErr) {Erreur( 62, iErr);	return;}
	
	GetPort( &myPort);
	
	TheDia = GetNewDialog( 168,NULL, (WindowPtr) -1L);
	SetPortDialogPort( TheDia);
	
	
	
	theCell.h = 0;
	theCell.v = whichItem;
	
	FileInfoID = whichItem;
	
	myFSS = *specList[ theCell.v];
	
	pStrcpy( FileInfoName, myFSS.name);
	
	SetDText( TheDia, 4, myFSS.name);
	SelectDialogItemText( TheDia, 4, 0, 32767);
	
	iErr = FSpGetFInfo( &myFSS, &fndrInfo);
	copy = fndrInfo;
	
	aStr[ 0] = 4;
	*((OSType*)(aStr+1)) = fndrInfo.fdType;
	SetDText( TheDia, 7, aStr);
	
	
	aStr[ 0] = 4;
	*((OSType*)(aStr+1)) = fndrInfo.fdCreator;
	SetDText( TheDia, 9, aStr);
	
	///////
	
	OSType2Ptr( fndrInfo.fdType, tempC);
	if( MADMusicIdentifyFSp( gMADLib, tempC, &myFSS) != noErr)
	{
	}
	else
	{
		PPInfoRec		info;
		
		MyP2CStr( FileInfoName);
		iErr = HSetVol( NULL, myFSS.vRefNum, myFSS.parID);
		iErr = PPInfoFile( gMADLib, tempC, (Ptr) FileInfoName, &info);
		MyC2PStr( (Ptr) FileInfoName);
		
		if( iErr == noErr)
		{
			MyC2PStr( info.internalFileName);
			SetDText( TheDia, 19, (unsigned char*) info.internalFileName);
			MyP2CStr( (unsigned char*) info.internalFileName);
			
			NumToString( info.totalPatterns, aStr);
			SetDText( TheDia, 20, aStr);
			
			MyC2PStr( info.formatDescription);
			SetDText( TheDia, 21, (unsigned char*) info.formatDescription);
			MyP2CStr( (unsigned char*) info.formatDescription);
			
			NumToString( info.totalInstruments, aStr);
			SetDText( TheDia, 22, aStr);

			NumToString( info.fileSize/1024L, aStr);
			pStrcat( aStr, "\p Kb");
			SetDText( TheDia, 23, aStr);
			
			PathNameFromDirID(		myFSS.parID,
									myFSS.vRefNum,
									aStr);
									
			SetDText( TheDia, 25, aStr);
		}
	}
	
	AutoPosition( TheDia);
	ChangeDialogFont( TheDia);
	
	do
	{
		EncoreEncore:
	//	ModalDialog( NULL, &itemHit);
		MyModalDialog( TheDia, &itemHit);
		
		switch( itemHit)
		{
			case -5:		// null Event
				GetDText( TheDia, 7, aStr);
				if( aStr[ 0] == 4)
				{
					if( *((OSType*) (aStr+1)) != fndrInfo.fdType)
					{
						fndrInfo.fdType = *((OSType*) (aStr+1));
						
						FSpSetFInfo( &myFSS, &fndrInfo);
						
						FileInfoIcon( TheDia);
					}
				}
				
				GetDText( TheDia, 9, aStr);
				if( aStr[ 0] == 4)
				{
					if( *((OSType*) (aStr+1)) != fndrInfo.fdCreator)
					{
						fndrInfo.fdCreator = *((OSType*) (aStr+1));
						
						FSpSetFInfo( &myFSS, &fndrInfo);
						
						FileInfoIcon( TheDia);
					}
				}
				
				GetDText( TheDia, 4, aStr);
				if( !EqualString( aStr, FileInfoName, true, true))
				{
					pStrcpy( FileInfoName, aStr);
					if( FileInfoName[ 0] > 31) FileInfoName[ 0] = 31;
					GetDialogItem( TheDia, 12, &itemType, &itemHandle, &itemRect);
					TETextBox( FileInfoName+1, FileInfoName[ 0], &itemRect, teCenter);
				}
			break;
		
			case 8:
				tempMenu = GetMenu( 158);
	
				InsertMenu( tempMenu, hierMenu);
				
				GetDialogItem( TheDia, itemHit, &itemType, &itemHandle, &itemRect);
				myPt.v = itemRect.top;	myPt.h = itemRect.left;
				LocalToGlobal( &myPt);
				
				GetDText( TheDia, 7, aStr);
				SetItemMark( tempMenu, FindTYPE( aStr, tempMenu), 0xa5);
				
				Result = PopUpMenuSelect(	tempMenu,
											myPt.v,
											myPt.h,
											FindTYPE( aStr, tempMenu));
				
			//	SetItemMark( tempMenu, curEData->amp / 8, 0);
				
				if ( HiWord( Result ) != 0 )
				{
					OSErr	iErr;
				
					iErr = FSpGetFInfo( &myFSS, &fndrInfo);
					
					fndrInfo.fdType = GetTYPE( LoWord( Result), tempMenu);
					
					iErr = FSpSetFInfo( &myFSS, &fndrInfo);
					if( iErr) Erreur( 75, iErr);
					else
					{
						aStr[ 0] = 4;
						*((OSType*)(aStr+1)) = fndrInfo.fdType;
						SetDText( TheDia, 7, aStr);
						SelectDialogItemText( TheDia, 7, 0, 32767);
					}
					FileInfoIcon( TheDia);
				}
				
				DeleteMenu( GetMenuID( tempMenu));
				DisposeMenu( tempMenu);
			break;
			
			case 10:
				tempMenu = GetMenu( 155);
	
				InsertMenu( tempMenu, hierMenu);
				
				GetDialogItem( TheDia, itemHit, &itemType, &itemHandle, &itemRect);
				myPt.v = itemRect.top;	myPt.h = itemRect.left;
				LocalToGlobal( &myPt);
				
				GetDText( TheDia, 9, aStr);
				SetItemMark( tempMenu, FindTYPE( aStr, tempMenu), 0xa5);
	
				Result = PopUpMenuSelect(	tempMenu,
											myPt.v,
											myPt.h,
											FindTYPE( aStr, tempMenu));
	
		//		SetItemMark( tempMenu, curEData->amp / 8, 0);
				
				if ( HiWord( Result ) != 0 )
				{
					OSErr	iErr;
				
					iErr = FSpGetFInfo( &myFSS, &fndrInfo);
					
					fndrInfo.fdCreator = GetTYPE( LoWord( Result), tempMenu);
					
					iErr = FSpSetFInfo( &myFSS, &fndrInfo);
					
					if( iErr) Erreur( 75, iErr);
					else
					{
						aStr[ 0] = 4;
						*((OSType*)(aStr+1)) = fndrInfo.fdCreator;
						SetDText( TheDia, 9, aStr);
						SelectDialogItemText( TheDia, 9, 0, 32767);
					}
					FileInfoIcon( TheDia);
				}
				
				DeleteMenu( GetMenuID( tempMenu));
				DisposeMenu( tempMenu);
			break;
		}
		
	}while( itemHit != 1 && itemHit != 2);

	if( itemHit == 1)
	{
		GetDText( TheDia, 4, aStr);
		if( aStr[ 0] > 31) aStr[ 0] = 31;
		
	//	HSetVol( NULL, myFSS.vRefNum, myFSS.parID);
		if( FSpRename( &myFSS, aStr) == noErr)
		{
			pStrcpy( myFSS.name, aStr);
		
			theCell.h = 0; theCell.v = whichItem;
		
			SetPortDialogPort( MODListDlog);
			BackColor( whiteColor);
			*specList[ theCell.v] = myFSS;
			
			
			myList.rect.right -=15;
			InvalWindowRect( GetDialogWindow( TheDia), &myList.rect);
			myList.rect.right +=15;
			
			RGBBackColor( &theColor);
			
			DoShowInfo( theCell);
		}
		else
		{
			Erreur( 75, 0);
			goto EncoreEncore;
		}
	}
	else
	{
		FSpSetFInfo( &myFSS, &copy);
	}
	
	DisposeDialog( TheDia);
	SetPort( myPort);
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_ARRAY_ELEMS	5
#define STRING_LENGTH	3

typedef struct QSort {
	FSSpec	 		*spec;
	unsigned char	*tt;
} QSort;

static int CompareFSSpec (const void *s1, const void *s2);
static int CompareFSSpec (const void *s1, const void *s2)
{
	return( RelString( ((QSort*) s1)->spec->name, ((QSort*) s2)->spec->name, false, false));
}

void SortMusicList(void)
{
	QSort			*myFSS;
	FSSpec			selectDat;
	short			i, temp;
	Point			theCell, selecCell;
	Str255			tempStr;

	myFSS = (QSort*) MyNewPtr( myList.maxY * sizeof( QSort));
	if( myFSS != NULL)
	{
		selecCell.h = 0;	selecCell.v = 0;
		if( PLGetSelect( &selecCell, &myList))
		{
			selectDat = *specList[ selecCell.v];
		}
		
		theCell.h = 0;	theCell.v = 0;
		for( i = 0; i < myList.maxY; i++)
		{
			myFSS[ i].spec = specList[ i];
			myFSS[ i].tt = pathStrList[ i];
		}
	
		qsort( myFSS, myList.maxY, sizeof( QSort), CompareFSSpec);
		
		theCell.h = 0;	theCell.v = 0;
		for( i = 0; i < myList.maxY; i++)
		{
			specList[ i] 	= myFSS[ i].spec;
			pathStrList[ i]	= myFSS[ i].tt;
		}
		
		/* Reselectionne la bonne cell */
		selecCell.h = 0;	selecCell.v = 0;
		if( PLGetSelect( &selecCell, &myList))
		{
			PLSetSelect( -1, -1, -1, -1, &myList);
			
			selecCell.h = 0;	selecCell.v = 0;
		/*	if( LSearch( &selectDat, sizeof( FSSpec), NULL, &selecCell, MODList))
			{
				PLSetSelect( selecCell.h, selecCell.v, selecCell.h, selecCell.v, &myList);
				PLAutoScroll( &myList);
			}	*/
			
			selectDat = *specList[ selecCell.v];
		}
		/***/
				
		myList.rect.right -=15;
		InvalWindowRect( GetDialogWindow( MODListDlog), &myList.rect);
		myList.rect.right +=15;

		MyDisposePtr( (Ptr*) &myFSS);
	}
}

void DoItemPressMODList( short whichItem, DialogPtr whichDialog)
{
		Cell				theCell;
 		short				dataLen,itemType,i, temp;
		Boolean				DoubleClick;
		Str255				StrTemp;
		Handle				itemHandle;
		Rect				caRect, itemRect, cellRect;
		OSErr				iErr;
//		StandardFileReply	reply;
		Point				myPt;
		long				tempLong;
		FInfo				fndrInfo;
		GrafPtr				SavePort;
		FSSpec				myFSS;
		
		GetPort( &SavePort);
 		SetPortDialogPort( whichDialog);
		
		if (theEvent.what == mouseDown)
		{
			myPt = theEvent.where;
			GlobalToLocal( &myPt);
			
			PLScroll( myPt, &myList);
			
			if( PtInRect( myPt, &myList.rect))
			{
				theCell.v = 0;	theCell.h = 0;
				if( PLGetSelect( &theCell, &myList)) temp = theCell.v;
				else temp = -1;
				
				DoubleClick = PLClick( myPt, theEvent.modifiers, &myList);
				
				PLGetSelectRect( &itemRect, &myList);
				if( PtInRect( myPt, &itemRect))
				{
					DragMODSelect();
				}
				
				theCell.v = 0;	theCell.h = 0;
				if( PLGetSelect( &theCell, &myList))
				{
						if( DoubleClick == false && theCell.v != temp)
						{
							iErr = DoShowInfo( theCell);
						}
						else if( DoubleClick == true)
						{
							DoKeyPressMODList( 0x0D);
							
						/*	DoPause();
							
							if( curMusic->hasChanged) 
							{	
								if( GereChanged() != noErr) goto OnRefaitEvent;
							}
							
							myFSS = *specList[ theCell.v];
							
							if( ImportFile( myFSS.name, myFSS.vRefNum, myFSS.parID, 0))
							{
								DoPlay();
							
								if( ((WindowPeek) ToolsDlog)->visible)
								{
									PLGetSelectRect( &cellRect, &myList);
								}
							}
							
							OnRefaitEvent:;*/
						}
					}
					else
					{
						EraseMODInfo();
						HiliteControl( DelBut, 255);
						HiliteControl( InfoBut, 255);
						HiliteControl( OpenBut, 255);
						
						if( DoubleClick)
						{
							HandleFileChoice( 2);
						}
					}
			}
			lastWhen = theEvent.when;
		}   						/* End of mouseDown */

	switch( whichItem)
	{	
	case 27:
	//	if( GetControlHilite( InfoBut) == 0 && MyTrackControl( InfoBut, theEvent.where, NULL))
		{
			theCell.v = 0;	theCell.h = 0;
			if( PLGetSelect( &theCell, &myList)) FileInformations( theCell.v);
		}
	break;
	
	case 32:
	//	if( GetControlHilite( AZBut) == 0 && MyTrackControl( AZBut, theEvent.where, NULL))
		{
			SortMusicList();
		}
	break;

	case 31:
	//	if( GetControlHilite( DelBut) == 0 && MyTrackControl( DelBut, theEvent.where, NULL))
		{
			DoKeyPressMODList( 8);
		}
	break;

	case 28:
	//	if( GetControlHilite( OpenBut) == 0 && MyTrackControl( OpenBut, theEvent.where, NULL))
		{
			DoKeyPressMODList( 0x0D);
		}
	break;
	
	case 29:
	//	if( GetControlHilite( LoadBut) == 0 && MyTrackControl( LoadBut, theEvent.where, NULL))
		{
			HandleFileChoice( 2);
		}
	break;

	case 19:
	//	if( GetControlHilite( prefBut) == 0 && MyTrackControl( prefBut, theEvent.where, NULL))
		{
#include "Help.h"
			
			ShowPrefs( MUSICLIST);
		}
	break;

	case 30:
	//	if( GetControlHilite( FlipBut) == 0 && MyTrackControl( FlipBut, theEvent.where, NULL))
		{
			EraseGrowIcon( whichDialog);
			
			GetPortBounds( GetDialogPort( whichDialog), &caRect);
			
			if( caRect.right == BIGMUSICLIST)
			{
				MySizeWindow( whichDialog, 200, caRect.bottom, true);
				SetControlValue( FlipBut, 1);
			}
			else
			{
				MySizeWindow( whichDialog, BIGMUSICLIST, caRect.bottom, true);
				SetControlValue( FlipBut, 0);
				
				theCell.h=theCell.v=0;
				if( PLGetSelect( &theCell, &myList)) DoShowInfo( theCell);
			}
			
			GetPortBounds( GetDialogPort( whichDialog), &caRect);
			
			EraseGrowIcon( whichDialog);
			SetMaxWindow( caRect.right + 3, 0, MODListDlog);
		}
	break;
	}
	
	SetPort( SavePort);
}

void ClearMODList()
{
	GrafPtr		SavePort;
	Rect		caRect, itemRect;
	Str255		aStr;

	GetPort( &SavePort);
	SetPortDialogPort( MODListDlog);
	
	MLDelRow( myList.maxY, 0);
	
	GetPortBounds( GetDialogPort( MODListDlog), &caRect);
	
	SetRect( &itemRect, 0, myList.rect.bottom, myList.rect.right-16, caRect.bottom);
	InvalWindowRect( GetDialogWindow( MODListDlog), &itemRect);
	
	PLAutoScroll( &myList);
	
	changedMusicList = false;
	curMusicListFile = false;
	pStrcpy( curMusicList.name, MLNONAME);
	
	SetPort( SavePort);
}

void SetMODListWindowName()
{
	Str255	aStr;
	
	pStrcpy( aStr, curMusicList.name);
	SetWTitle( GetDialogWindow( MODListDlog), aStr);
}

OSErr CheckFileAvailable( short ID)
{
	FInfo					fndrInfo;
	OSErr					iErr;
	FSSpec					reply;
	Str255					aStr;
	
	iErr = FSpGetFInfo( specList[ ID], &fndrInfo);
	if( iErr == fnfErr || iErr == fLckdErr)
	{
		WDPBRec		wdpb;
		
		wdpb.ioNamePtr = pathStrList[ ID];
		iErr = PBHSetVol( &wdpb, false);
		if( iErr == noErr) HGetVol( NULL, &specList[ ID]->vRefNum, &specList[ ID]->parID);
		else {specList[ ID]->vRefNum	= 0;	specList[ ID]->parID	= 0;}
	}
	
	iErr = FSpGetFInfo( specList[ ID], &fndrInfo);
	if( iErr == fnfErr || iErr == fLckdErr)
	{
		short		itemHit;
		DialogPtr	aDia;
		GrafPtr		savedPort;
		
		GetPort( &savedPort);
		
		ParamText( specList[ ID]->name, NULL, NULL, NULL);
		
		aDia = GetNewDialog( 3003,NULL, (WindowPtr) -1L);
		SetPortDialogPort( aDia);
		
		AutoPosition( aDia);
		
		do
		{
			//ModalDialog( MyDlgFilterDesc, &itemHit);
			MyModalDialog( aDia, &itemHit);
		
		}while( itemHit != 1 && itemHit != 2 && itemHit != 3);
	
		DisposeDialog( aDia);
		SetPort( savedPort);
		UpdateALLWindow();
		
		switch( itemHit)
		{
			case 1:			// Remove
				MLDelRow( 1, ID);
				return -1;
			break;
			
			case 2:			// Find
				{
					if( NGetFileName( &reply) != noErr) return -1;
					
					*specList[ ID] = reply;
					
					PLSetSelect( -1, -1, -1, -1, &myList);
					PLSetSelect( 0, ID, 0, ID, &myList);
					
					return noErr;
				}
			break;
			
			case 3:			// Cancel
				return -1;
			break;
		}
		
		return noErr;
	}
	
	return noErr;
}

void DoKeyPressMODList( short theChar)
{
	GrafPtr		SavePort;
	Point		theCell;
	Str255		StrTemp;
	Rect		cellRect, itemRect;
	short		i, dataLen, rowDelete, firstSelec;
	Boolean		Redraw;
	FSSpec		myFSS;
	
	if( MODListDlog == NULL) return;
	
	GetPort( &SavePort);
	SetPortDialogPort( MODListDlog);
	
	PLDoArrows( theChar, &myList);
	
	if( theChar == deletekey)
	{
		theCell.v = 0;	theCell.h = 0;
		if( PLGetSelect( &theCell, &myList))
		{
			rowDelete = myList.select.bottom - myList.select.top + 1;
			
			if( rowDelete > 0)
			{
				theCell.v = 0;	theCell.h = 0;
				PLGetSelect( &theCell, &myList);
				
				GetKeys( km);
				if( IsPressed( 0x0037))
				{
					if( IsPressed( 0x0038))
					{
						if( InfoL( 102))
						{
							FSpDelete( specList[ theCell.v]);
							MLDelRow( rowDelete, theCell.v);
						}
					}
					else
					{
						FSSpec	trashSpec;
						OSErr	iErr;
						short	vRefNum;
						
						iErr = FindFolder( specList[ theCell.v]->vRefNum, kTrashFolderType, kCreateFolder, &trashSpec.vRefNum, &trashSpec.parID);
						
						if( iErr == noErr)
						{
							FSSpec		tempFile;
							CMovePBRec	destDir;
							
							tempFile = *specList[ theCell.v];
							
							trashSpec.vRefNum = tempFile.vRefNum;
							
							destDir.	ioCompletion = NULL;
							destDir.	ioNamePtr = tempFile.name;
							destDir.	ioVRefNum = tempFile.vRefNum;
							destDir.	ioNewName = NULL;	//trashSpec.name;
							destDir.	ioNewDirID = trashSpec.parID;
							destDir.	ioDirID = tempFile.parID;
							
							iErr = PBCatMoveSync( &destDir);
							
							MLDelRow( rowDelete, theCell.v);
						}
					}
				}
				else MLDelRow( rowDelete, theCell.v);
			}
			
			PLSetSelect( 0, theCell.v, 0,theCell.v, &myList);
			PLAutoScroll( &myList);
			UpdateMODListWindow( MODListDlog);
		}
	}
	else if( theChar == 0x0D || theChar == 0x03)
	{
		theCell.v = 0;	theCell.h = 0;
		if( PLGetSelect( &theCell, &myList))
		{
			DoPause();
							
			if( GereChanged() != noErr) goto OnRefaitEvent;
			
			if( CheckFileAvailable( theCell.v) == noErr)
			{
				myFSS = *specList[ theCell.v];
				
				if( ImportFile( myFSS.name, myFSS.vRefNum, myFSS.parID, 0))
				{
					if( thePrefs.AutoPlayWhenOpen) DoPlay();
				}
				else
				{
					//SelectWindow2( MODListDlog);
				}
			}
			OnRefaitEvent:;
		}
	}
	else if( theChar == selectAll)
 	{
 		myList.select.top = 0;
 		myList.select.bottom = myList.maxY-1;
 		myList.select.left = 0;
 		myList.select.right = 1;
 		
 		PLGetSelectRect( &cellRect, &myList);
		InvalWindowRect( GetDialogWindow( MODListDlog), &cellRect);
 	}
	else if( theChar == getinfo)
	{
		theCell.v = 0;	theCell.h = 0;
		if( PLGetSelect( &theCell, &myList)) FileInformations( theCell.v);
	}
	else MyKeySearchInList( theChar);

	theCell.v = 0;	theCell.h = 0;
	if( PLGetSelect( &theCell, &myList)) DoShowInfo( theCell);
	else
	{
		EraseMODInfo();
		HiliteControl( DelBut, 255);
		HiliteControl( InfoBut, 255);
		HiliteControl( OpenBut, 255);
	}
	SetPort( SavePort);
}

void DoLoadOtherMusic( Boolean Affiche)
{
	Point		theCell, cSize;
	OSErr		iErr;
	short		LastCopy, dataLen;
	GrafPtr		SavePort;
	Byte		ttt;
	FSSpec		myFSS;
	Boolean		loaded;

	if( myList.maxY <= 0) return;
	
	if( thePrefs.LoopType == 4) // Will be handle later...
	{
		return;
	}
	
	MADDriver->musicEnd = false;
	if( thePrefs.LoopType == 0) return;
	
	DoStop();
	
	if( thePrefs.LoopType == 3)
	{
		MADReset( MADDriver);
		return;
	}
	
	if( Affiche)
	{
		if( curMusic->hasChanged) 
		{	
			if( GereChanged() != noErr) return;
		}
		
		GetPort( &SavePort);
		SetPortDialogPort( MODListDlog);
	}
	
	theCell.v = 0;		theCell.h = 0;
	theCell.v = IsAlreadyHere( lastLoadMODListName);
	
	LastCopy = theCell.v;
	ReRandom:
	switch( thePrefs.LoopType)
	{
		case 0:			// repeat
			MyDebugStr( __LINE__, __FILE__, "What are you doing here??");
			return;
		break;
		
		case 1:			// load next
			theCell.v++;
			if( theCell.v >= myList.maxY) theCell.v = 0;
		break;
		
		case 2:			// shuffle
			ttt = TickCount();
			while( ttt > 0) { Random(); ttt--;}
			theCell.v = RangedRdm( 0, myList.maxY);
			if( theCell.v < 0 || theCell.v >= myList.maxY) theCell.v = 0;
		break;
	}
	
	if( LastCopy == theCell.v)
	{
		if( myList.maxY > 1) goto ReRandom;
		else
		{
			MADReset( MADDriver);
			DoPlay();
			return;
		}
	}

	if( Affiche)
	{
		BackColor( whiteColor);
		PLSetSelect( -1, -1, -1, -1, &myList);
		
		PLSetSelect( theCell.h, theCell.v, theCell.h, theCell.v, &myList);
		PLAutoScroll( &myList);
		RGBBackColor( &theColor);
		
		iErr = DoShowInfo( theCell);
	}
	
	myFSS = *specList[ theCell.v];
	
	MADReset( MADDriver);
	
	loaded = ImportFile( myFSS.name, myFSS.vRefNum, myFSS.parID, 0);
	
	MADReset( MADDriver);
	if( thePrefs.AutoPlayWhenOpen && loaded) DoPlay();
	
	if( Affiche) SetPort( SavePort);
}

void SetcurMusicListFile( Boolean	val)
{
	curMusicListFile = val;
}

OSErr GereMusicListChanged()
{
	short					itemHit;
	DialogPtr				aDia;
	GrafPtr					savedPort;
	ProcessSerialNumber		PSN;

	if( changedMusicList == false) return noErr;
	if( thePrefs.SaveMusicList == false) return noErr;
	
/*	GetFrontProcess( &PSN);
	if( PSN.highLongOfPSN != playerPROPSN.highLongOfPSN ||
		PSN.lowLongOfPSN != playerPROPSN.lowLongOfPSN) return noErr;
*/
	GetPort( &savedPort);
	
	ParamText( curMusicList.name, NULL, NULL, NULL);
	
	aDia = GetNewDialog( 3004,NULL, (WindowPtr) -1L);
	SetPortDialogPort( aDia);
	
	AutoPosition( aDia);
	
	do
	{
		//ModalDialog( MyDlgFilterDesc, &itemHit);
		MyModalDialog( aDia, &itemHit);
		
	}while( itemHit != 1 && itemHit != 2 && itemHit != 3);
	
	DisposeDialog( aDia);
	SetPort( savedPort);
	UpdateALLWindow();
	
	switch( itemHit)
	{
		case 1:
		{
			short	ss;
			long	ll;
		
			HGetVol( NULL, &ss, &ll);
			
			if( curMusicListFile)
			{
				//HSetVol( NULL, curMusicList.vRefNum, curMusicList.parID);
				SaveMyMODListSTCf( curMusicList);
			}
			else SaveMODList();
			
			HSetVol( NULL, ss, ll);
			
			return noErr;
		}
		break;
		
		case 2:
			changedMusicList = false;
			return noErr;
		break;
		
		case 3:
			return -1;
		break;
	}
	
	return noErr;
}

void DoNullMODList()
{
/*Rect		tRect;
Point		pt;
GrafPtr		savedPort;

if( MODListDlog == NULL) return;

	GetPort( &savedPort);
	SetPortDialogPort( MODListDlog);
	
if( oldWindow == MODListDlog)
{
	PLGetSelectRect( &tRect, &myList);
	GetMouse( &pt);
	if( PtInRect( pt, &tRect))
	{
		SetCursor( &HandCrsr);
	}
	else SetCursor( GetQDGlobalsArrow( &qdarrow));
}
SetPort( savedPort);*/
}

void ReconstructShowInfo()
{
	Point	theCell;
	GrafPtr	savedPort;
	
	if( MODListDlog == NULL) return;
	
	GetPort( &savedPort);
	SetPortDialogPort( MODListDlog);
	
	theCell.h = theCell.v = 0;
	if( PLGetSelect( &theCell, &myList)) DoShowInfo( theCell);
	
	SetPort( savedPort);
}

void COPYMODList()		// Create a rsrc from a file
{
	Point		theCell;
	OSErr		anErr;
	ScrapRef	scrap;

	GetKeys( km);
	
	if( IsPressed( 0x31))
	{
		theCell.v = 0;	theCell.h = 0;
		if( PLGetSelect( &theCell, &myList))
		{
			if( CheckFileAvailable( theCell.v) == noErr)
			{
				FSSpec	myFSS = *specList[ theCell.v];
				Ptr		file;
				OSType	type;
				FInfo	fndrInfo;
				short	refNum;
				long	inOutCount;
				
				FSpGetFInfo( &myFSS, &fndrInfo);
				
				FSpOpenDF( &myFSS, fsCurPerm, &refNum);
				GetEOF( refNum, &inOutCount);
				
				file = NewPtr( inOutCount);
				if( file != NULL)
				{
					FSRead( refNum, &inOutCount, file);
										
					anErr = ClearCurrentScrap();
					
					anErr = GetCurrentScrap( &scrap);
					
					anErr = PutScrapFlavor( scrap, fndrInfo.fdType, 0, inOutCount, file);
					
					DisposePtr( file);
				}
				FSCloseFork( refNum);
			}
		}
	}
}

void PASTEMODList()		// Create a file from a handle - 
{
/*	long 			lCntOrErr, scrapOffset;
	PScrapStuff		pScrpInf;
	
	GetKeys( km);
	
	if( IsPressed( 0x31))
	{
		pScrpInf = InfoScrap();
		
		if( pScrpInf->scrapSize > 0)
		{
			lCntOrErr = GetScrap( NULL, *((OSType*)*pScrpInf->scrapHandle), &scrapOffset);
			
			if( lCntOrErr > 0)
			{
				FSSpec				reply;
				short				refNum;
				Handle				theHandle;
				Str255				str, str2;
				long				inOutCount;
				OSErr				iErr;
				
				theHandle = MyNewHandle( lCntOrErr);
				if( theHandle != NULL)
				{
					GetScrap( theHandle, *((OSType*)*pScrpInf->scrapHandle), &scrapOffset);
					HLock( theHandle);
					
					OSType2Str( *((OSType*)*pScrpInf->scrapHandle), str);
					pStrcat( str, "\p-");
					NumToString( lCntOrErr, str2);
					pStrcat( str, str2);
					pStrcat( str, "\p Bytes");
					
					iErr = DoCustomSave( "\pCreate a file", "\pTEST", *((OSType*)*pScrpInf->scrapHandle), &reply);
					
					if( iErr == noErr)
					{
						FSpDelete( &reply);
						FSpCreate( &reply, 'SNPL',  *((OSType*)*pScrpInf->scrapHandle), smSystemScript);
						FSpOpenDF( &reply, fsCurPerm, &refNum);
						
						inOutCount = lCntOrErr;
						FSWrite( refNum, &inOutCount, *theHandle);
						
						FSClose( refNum);
						
						HUnlock( theHandle);
						MyDisposHandle( & theHandle);
						
						AddMODList( true,
									reply.name,
									reply.vRefNum,
									reply.parID);
					}
				}
			}
		}
	}*/
}