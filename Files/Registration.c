#include "Shuddup.h"#include "Rdriver.h"#include "Traps.h"#include "mixedmode.h"typedef	struct{	short	Multiple;	short	Decompil;	}	Codage;static		Codage		theCode;static		Handle		thePlug;static		short		CodeOK;static		UniversalProcPtr theProctectionCall;Boolean PageSetUp();void SetDText (DialogPtr dlog, short item, Str255 str);void PubDialog( void);long PrefsUpdate();Boolean IsCodeOK();OSErr CallPlug( short item);long NewPrefAnalyse( Str255);long NewPrefAnalyse2( Str255 str);typedef void (*MyProcPtr) ( short);void DecodeText( Ptr	Source, long size, Byte	CCC);#define TrapMask 0x0800void CheckUsedTime(void){unsigned long	tempUL;	if( IsCodeOK())	{		GetDateTime( &tempUL);		tempUL -= thePrefs.firstStart;		if( tempUL > 60UL*60UL*24UL*20UL)		{			Erreur( 41, -69);				PubDialog();				if( IsCodeOK())			{			 	End = true;			}		}	}}void UNREGISTRED(void){	Erreur( 12, -5);}Boolean IsCodeOK(){	#ifdef DEMO		return true;	#else		return CodeOK;	#endif}void ProtectInit(void){long	RsrcS;	#ifdef DEMO	thePlug = 0L;	CodeOK = 1;		return;	#endif		if( thePrefs.Registred)	{		if( PrefsUpdate() == 0) DeletePreferences();				thePlug = GetResource('Burk', 1000);		if( thePlug == 0L) ExitToShell();		RsrcS = GetResourceSizeOnDisk( thePlug);				DetachResource( thePlug);		HNoPurge( thePlug);		HLockHi( thePlug);				DecodeText( *thePlug, RsrcS, thePrefs.Decode);				CodeOK = 0;			/*	#if defined(powerc) || defined (__powerc)				#define 	MyCallMode ( kCStackBased|\					RESULT_SIZE( SIZE_CODE( sizeof( short)))|\					STACK_ROUTINE_PARAMETER( 1, SIZE_CODE( sizeof( short))))						theProctectionCall = NewRoutineDescriptor( (ProcPtr) *thePlug, MyCallMode, kM68kISA);		#endif*/				if( PrefsUpdate() == 0)		{			Erreur( 90, 90);			ExitToShell();		}	}	else	{		thePlug = 0L;		CodeOK = 1;	}}void DecodeText( Ptr	Source, long size, Byte	CCC){long	i;Source[0] -= CCC;for( i=1; i < size; i++){	Source[i] += Source[i - 1];}}OSErr CallPlug( short item)					// CODE du plug{	#ifdef DEMO		UNREGISTRED();		return 1;	#else	if( CodeOK)	{		UNREGISTRED();		return CodeOK;	}	//	FSClose( item);	/*	#if defined(powerc) || defined (__powerc)		CallUniversalProc( theProctectionCall, MyCallMode, item);	#else		(* (MyProcPtr) (*thePlug)) ( item);	#endif*/	#endif	return	CodeOK;}/*OSErr TestCode(void){OSErr	err = noErr;	if( theCode.Multiple == 0) return 1;	else if( theCode.Multiple % 17 != 0) return 1;	else return noErr;}*/static	Str255		Addresse1, Addresse2, Addresse3, Fname, Lname, Country, Phone;static	long		distributor;void PrintRegistration(void){	PicHandle	thePict;	Rect		caRect, tempRect, picRect;	short		Larg, Haut, iRefNum, pos;	GrafPtr		myPort;		iRefNum = CurResFile();		GetPort( &myPort);		GetPortBounds( (CGrafPtr) myPort, &caRect);		tempRect.left = caRect.left;		tempRect.right = caRect.right;	tempRect.top = caRect.top;		tempRect.bottom = caRect.bottom;		if( distributor < 8)		thePict = GetPicture( 132);	else						thePict = GetPicture( 142);		picRect.left = (*thePict)->picFrame.left;		picRect.right = (*thePict)->picFrame.right;	picRect.top = (*thePict)->picFrame.top;			picRect.bottom = (*thePict)->picFrame.bottom;		Larg = picRect.right - picRect.left;	Haut = picRect.bottom - picRect.top;		picRect.left = tempRect.left + - Larg/2 + (tempRect.right - tempRect.left)/2;	picRect.right = picRect.left + Larg;		picRect.top = tempRect.top + - Haut/2 + (tempRect.bottom - tempRect.top)/2;	picRect.bottom = picRect.top + Haut;		DrawPicture( thePict, &picRect);	/*	switch( Disk)	{		case 4: SetRect( &tempRect, picRect.left + 78, picRect.top + 90, picRect.left + 86, picRect.top + 98);		break;		case 5: SetRect( &tempRect, picRect.left + 25, picRect.top + 115, picRect.left + 33, picRect.top + 123);	break;	}	PaintRect( &tempRect);	switch( Payment)	{		case 7: SetRect( &tempRect, picRect.left + 73, picRect.top + 165, picRect.left + 81, picRect.top + 173);	break;		case 8: SetRect( &tempRect, picRect.left + 28, picRect.top + 188, picRect.left + 36, picRect.top + 196);	break;		case 9: SetRect( &tempRect, picRect.left + 30, picRect.top + 213, picRect.left + 38, picRect.top + 221);	break;	}	PaintRect( &tempRect);*/	TextFace( bold);	TextFont( kFontIDHelvetica);	TextSize( 12);		#define INTERL 23		pos = 305;		MoveTo( picRect.left + 185, picRect.top + pos);		DrawString( Fname);			pos	+= INTERL;	MoveTo( picRect.left + 185, picRect.top + pos);		DrawString( Lname);			pos	+= INTERL;	MoveTo( picRect.left + 185, picRect.top + pos);		DrawString( Phone);			pos	+= INTERL;	MoveTo( picRect.left + 185, picRect.top + pos);		DrawString( Addresse1);		pos	+= INTERL;	MoveTo( picRect.left + 185, picRect.top + pos);		DrawString( Addresse2);		pos	+= INTERL;	MoveTo( picRect.left + 185, picRect.top + pos);		DrawString( Addresse3);		pos	+= INTERL;	MoveTo( picRect.left + 185, picRect.top + pos);		DrawString( Country);		pos	+= INTERL;	TextFace( 0);		ReleaseResource( (Handle) thePict);		UseResFile( iRefNum);}void InitPrintRegistration(void){	pStrcpy( Addresse1, "\p");	pStrcpy( Addresse2, "\p");	pStrcpy( Addresse3, "\p");	pStrcpy( Fname, "\p");	pStrcpy( Phone, "\p");	pStrcpy( Lname, "\p");	pStrcpy( Country, "\p");	/*	Disk = 4;	Payment = 7;	*/}void PreparePrintRegistration(void){	long		Result;	short		itemHit, i;	DialogPtr	TheDia;	Str255		theStr, UserName;	GrafPtr		myPort;	MenuHandle	tMenu;	Point		myPt;	Handle		itemHandle;	Rect		itemRect;	short		itemType;	long		mresult;		return;		GetPort( &myPort);	TheDia = GetNewDialog( 174,0L, (WindowPtr) -1L);	SetPortDialogPort( TheDia);	ChangeDialogFont( TheDia);//	AutoPosition( TheDia);		distributor = 0;		ShowWindow( GetDialogWindow( TheDia));//	TurnRadio( Disk, TheDia, true);//	TurnRadio( Payment, TheDia, true);	SetDText( TheDia, 7, Fname);	SetDText( TheDia, 8, Lname);	SetDText( TheDia, 20, Phone);	SetDText( TheDia, 9, Addresse1);	SetDText( TheDia, 10, Addresse2);	SetDText( TheDia, 11, Addresse3);	SetDText( TheDia, 13, Country);	do	{		//ModalDialog( MyDlgFilterDesc, &itemHit);		MyModalDialog( TheDia, &itemHit);				switch( itemHit)		{			case 16:				tMenu = GetMenu( 164);				InsertMenu( tMenu, hierMenu );				GetDialogItem( TheDia, itemHit, &itemType, &itemHandle, &itemRect);								myPt.v = itemRect.top;	myPt.h = itemRect.left;				LocalToGlobal( &myPt);								mresult = PopUpMenuSelect(	tMenu,											myPt.v,											myPt.h,											1);								if( HiWord( mresult) != 0)				{					distributor = LoWord( mresult);									if( LoWord( mresult) < 8)					{						SetDText( TheDia, 15, "\pUSA Distributor");					}					else if( LoWord( mresult) < 10)					{						SetDText( TheDia, 15, "\pJAPAN Distributor");					}					else if( LoWord( mresult) == 11)					{						SetDText( TheDia, 15, "\pUK Distributor");					}					else if( LoWord( mresult) == 12)					{						SetDText( TheDia, 15, "\pFrance Distributor");					}					else					{						SetDText( TheDia, 15, "\pEurope Distributor");					}				}				DeleteMenu( GetMenuID( tMenu));				DisposeMenu( tMenu);			break;						case 1:				GetDText( TheDia, 7, (StringPtr) &Fname);				GetDText( TheDia, 8, (StringPtr) &Lname);				GetDText( TheDia, 20, (StringPtr) &Phone);				GetDText( TheDia, 9, (StringPtr) &Addresse1);				GetDText( TheDia, 10, (StringPtr) &Addresse2);				GetDText( TheDia, 11, (StringPtr) &Addresse3);				GetDText( TheDia, 13, (StringPtr) &Country);								if( Country[ 0] == 0)				{					Erreur( 84, -5);				}				else if( distributor == 0)				{					Erreur( 85, -6);				}				else if( PageSetUp())				{					Print();				}								SetPortDialogPort( TheDia);			break;		}	}while(itemHit != 2);		GetDText( TheDia, 7, (StringPtr) &Fname);	GetDText( TheDia, 8, (StringPtr) &Lname);	GetDText( TheDia, 9, (StringPtr) &Addresse1);	GetDText( TheDia, 10, (StringPtr) &Addresse2);	GetDText( TheDia, 11, (StringPtr) &Addresse3);	GetDText( TheDia, 13, (StringPtr) &Country);	GetDText( TheDia, 20, (StringPtr) &Phone);	DisposDialog( TheDia);	SetPort( myPort);	}void EraseResources( OSType type, short ID){Handle	aRsrc;	aRsrc = GetResource( type, ID);	RemoveResource( aRsrc);	DisposHandle( aRsrc);		UpdateResFile( CurResFile());}OSErr TestNewCode( Str255 str){	if(	str[ 2] == 'I' &&		str[ 3] == 'D' &&		str[ 4] == '4') return noErr;			else return -1;}OSErr TestNewCode2( Str255 str){	if(	str[ 2] == 'A' &&		str[ 3] == 'D' &&		str[ 4] == '5') return noErr;			else return -1;}void GenerateCodeNewID( Str255 strID, long num){	Str32	tStr;	Str32	Div19;	long	microVal, i, tt;		pStrcpy( strID, "\pCID4TTTTTTTTTTTTTTTTTTTTT");		strID[ 0] = 4 + 6 + 5;		// -------------		tt = num * 13;	//	pStrcpy( Div19, "\pHHHHHHHHHHHHHHHHHHHH");//	NumToString( tt, Div19);		sprintf( (Ptr) Div19, "%6ld", tt);	for( i = 0; i < 6; i++) if( Div19[ i] == ' ') Div19[ i] = '0';		Div19[ 0] = (Div19[ 0] - 0x30) + 0x51;	Div19[ 1] = (Div19[ 1] - 0x30) + 0x50;	Div19[ 2] = (Div19[ 2] - 0x30) + 0x51;	Div19[ 3] = (Div19[ 3] - 0x30) + 0x50;	Div19[ 4] = (Div19[ 4] - 0x30) + 0x51;	Div19[ 5] = (Div19[ 5] - 0x30) + 0x51;		microVal = tt & 0x0000000F;		// -------------		tt = 0x38;	tt *= 0x21;		NumToString( tt, tStr);		tStr[ 0] = (tStr[ 1] - 0x30) + 0x50 - microVal;	tStr[ 1] = (tStr[ 2] - 0x30) + 0x51 - microVal;	tStr[ 2] = (tStr[ 3] - 0x30) + 0x50 - microVal;	tStr[ 3] = (tStr[ 4] - 0x30) + 0x51 - microVal;		for( i = 0; i < 4; i++) strID[ 5 + i*2] = tStr[ i];		// UNE LETTRE SUR 2 !!	for( i = 0; i < 6; i++) strID[ 6 + i*2] = Div19[ i];	// UNE LETTRE SUR 2 !!	strID[ 6 + 9] = Div19[ 5];		if( NewPrefAnalyse( strID) != 0x38) {SysBeep( 1); MyDebugStr( __LINE__, __FILE__, "");}}void GenerateCodeNewID2( Str255 strID, long num){	Str32	tStr;	Str32	Div19;	long	microVal, i, tt;		pStrcpy( strID, "\pCAD5TTTTTTTTTTTTTTTTTTTTT");		strID[ 0] = 4 + 6 + 5;		// -------------		tt = num * 17;	//	pStrcpy( Div19, "\pHHHHHHHHHHHHHHHHHHHH");//	NumToString( tt, Div19);		sprintf( (Ptr) Div19, "%6ld", tt);	for( i = 0; i < 6; i++) if( Div19[ i] == ' ') Div19[ i] = '0';		Div19[ 0] = (Div19[ 0] - 0x30) + 0x51;	Div19[ 1] = (Div19[ 1] - 0x30) + 0x50;	Div19[ 2] = (Div19[ 2] - 0x30) + 0x51;	Div19[ 3] = (Div19[ 3] - 0x30) + 0x50;	Div19[ 4] = (Div19[ 4] - 0x30) + 0x51;	Div19[ 5] = (Div19[ 5] - 0x30) + 0x51;		microVal = tt & 0x0000000F;		// -------------		tt = 0x38;	tt *= 0x21;		NumToString( tt, tStr);		tStr[ 0] = (tStr[ 1] - 0x30) + 0x50 - microVal;	tStr[ 1] = (tStr[ 2] - 0x30) + 0x51 - microVal;	tStr[ 2] = (tStr[ 3] - 0x30) + 0x50 - microVal;	tStr[ 3] = (tStr[ 4] - 0x30) + 0x51 - microVal;		for( i = 0; i < 4; i++) strID[ 5 + i*2] = tStr[ i];		// UNE LETTRE SUR 2 !!	for( i = 0; i < 6; i++) strID[ 6 + i*2] = Div19[ i];	// UNE LETTRE SUR 2 !!	strID[ 6 + 9] = Div19[ 5];		if( NewPrefAnalyse2( strID) != 0x38) {SysBeep( 1); MyDebugStr( __LINE__, __FILE__, "");}}long NewPrefAnalyse( Str255 str){	if(	TestNewCode( str) == noErr)	{		Str32		tStr, Div19;		long		i;		long 		tt, ttc;		short		microVal;				for( i = 0; i < 4; i++) tStr[ i] = str[ 5 + i*2];		// UNE LETTRE SUR 2 !!		for( i = 0; i < 6; i++) Div19[ i] = str[ 6 + i*2];	// UNE LETTRE SUR 2 !!		Div19[ 5] = str[ 6 + 9];				Div19[ 0] = (Div19[ 0] + 0x30) - 0x51;		Div19[ 1] = (Div19[ 1] + 0x30) - 0x50;		Div19[ 2] = (Div19[ 2] + 0x30) - 0x51;		Div19[ 3] = (Div19[ 3] + 0x30) - 0x50;		Div19[ 4] = (Div19[ 4] + 0x30) - 0x51;		Div19[ 5] = (Div19[ 5] + 0x30) - 0x51;		Div19[ 6] = 0;				tt = ttc = atol( (Ptr) Div19);				tt /= 13;			// Multiple de 13 !				tt *= 13;				if( tt != ttc) return 0;				microVal = tt & 0x0000000F;				tStr[ 0] = (tStr[ 0] + 0x30) - 0x50 + microVal;	// Lettre, a partir de 'P'		tStr[ 1] = (tStr[ 1] + 0x30) - 0x51 + microVal;	// Lettre		tStr[ 2] = (tStr[ 2] + 0x30) - 0x50 + microVal;	// Numero		tStr[ 3] = (tStr[ 3] + 0x30) - 0x51 + microVal;	// Lettre, a partir de 'U'				tStr[ 4] = 0;				tt = atol( (Ptr) tStr);				tt /= 0x21;		// --> 0x38				return tt;	}		return 0;}long NewPrefAnalyse2( Str255 str){	if(	TestNewCode2( str) == noErr)	{		Str32		tStr, Div19;		long		i;		long 		tt, ttc;		short		microVal;				for( i = 0; i < 4; i++) tStr[ i] = str[ 5 + i*2];		// UNE LETTRE SUR 2 !!		for( i = 0; i < 6; i++) Div19[ i] = str[ 6 + i*2];	// UNE LETTRE SUR 2 !!		Div19[ 5] = str[ 6 + 9];				Div19[ 0] = (Div19[ 0] + 0x30) - 0x51;		Div19[ 1] = (Div19[ 1] + 0x30) - 0x50;		Div19[ 2] = (Div19[ 2] + 0x30) - 0x51;		Div19[ 3] = (Div19[ 3] + 0x30) - 0x50;		Div19[ 4] = (Div19[ 4] + 0x30) - 0x51;		Div19[ 5] = (Div19[ 5] + 0x30) - 0x51;		Div19[ 6] = 0;				tt = ttc = atol( (Ptr) Div19);				tt /= 17;			// Multiple de 13 !				tt *= 17;				if( tt != ttc) return 0;				microVal = tt & 0x0000000F;				tStr[ 0] = (tStr[ 0] + 0x30) - 0x50 + microVal;	// Lettre, a partir de 'P'		tStr[ 1] = (tStr[ 1] + 0x30) - 0x51 + microVal;	// Lettre		tStr[ 2] = (tStr[ 2] + 0x30) - 0x50 + microVal;	// Numero		tStr[ 3] = (tStr[ 3] + 0x30) - 0x51 + microVal;	// Lettre, a partir de 'U'				tStr[ 4] = 0;				tt = atol( (Ptr) tStr);				tt /= 0x21;		// --> 0x38				return tt;	}		return 0;}long PrefsUpdate(){	if( thePrefs.NewPrefSystem)	{		long tt;				thePrefs.NewPrefsCode[ 0] = 15;		if( RelString( thePrefs.NewPrefsCode, "\pCID4KQSPNSSRTWS", true, true) == 0) return 0;		if( RelString( thePrefs.NewPrefsCode, "\pCID4DQLPGULSTYR", true, true) == 0) return 0;		if( RelString( thePrefs.NewPrefsCode, "\pCID4GQOPJWOXTZQ", true, true) == 0) return 0;		if( RelString( thePrefs.NewPrefsCode, "\pCID4KQSPNSSRTWS", true, true) == 0) return 0;		if( RelString( thePrefs.NewPrefsCode, "\pCID4KQSPNSSRTWS2", true, true) == 0) return 0;		if( RelString( thePrefs.NewPrefsCode, "\pCID4CQKPFRKSTSW", true, true) == 0) return 0;		if( RelString( thePrefs.NewPrefsCode, "\pCAD5MQUPPSURTUU", true, true) == 0) return 0;		if( RelString( thePrefs.NewPrefsCode, "\pCAD5GQOPJROXTQS", true, true) == 0) return 0;		if( RelString( thePrefs.NewPrefsCode, "\pCID4PQXPSQXVTYZ", true, true) == 0) return 0;				if( TestNewCode2( thePrefs.NewPrefsCode) == noErr) tt = NewPrefAnalyse2( thePrefs.NewPrefsCode);		else tt = NewPrefAnalyse( thePrefs.NewPrefsCode);				return tt;	}	return 0;}void Registration(void){	long		Result, temp;	short		itemHit, i;	DialogPtr	TheDia;	Str255		theStr, UserName;	GrafPtr		myPort;		if( IsCodeOK() == 0)	{		Erreur( 31, -4);		return;	}		GetPort( &myPort);	TheDia = GetNewDialog( 175,0L, (WindowPtr) -1L);		SetPortDialogPort( TheDia);		ShowWindow( GetDialogWindow( TheDia));	ReGotoREGI:		temp = 43;		do	{		//ModalDialog( MyDlgFilterDesc, &itemHit);		MyModalDialog( TheDia, &itemHit);				switch( itemHit)		{		/*	case 8:				GenerateCodeNewID( theStr, temp += 10);								pStrcpy( thePrefs.NewPrefsCode, theStr);								if( NewPrefAnalyse( thePrefs.NewPrefsCode) != 0x38) SysBeep( 1);								SetDText( TheDia, 3, theStr);			break;*/		}			}while( itemHit != 1 && itemHit != 2);		if( itemHit == 1)	{		GetDText( TheDia, 4, UserName);		if( UserName[ 0] < 3) { Erreur( 30, -1);		goto ReGotoREGI; }				GetDText( TheDia, 3, (StringPtr) &theStr);				UpperString( theStr, true);				for( i = 1; i <= theStr[ 0]; i++)		{			if( theStr[ i] == ' ') { Erreur( 97, -1);		goto ReGotoREGI; }		}				if( theStr[ 0] < 8)		{ Erreur( 29, -1);		goto ReGotoREGI; }				if( theStr[ 0] > 25)	{ Erreur( 29, -1);		goto ReGotoREGI; }				if( theStr[ 1] == 'C')		// NEW code system !!!!!		{			if( TestNewCode( theStr) != noErr)			{				if( TestNewCode2( theStr) != noErr)				{Erreur( 29, -1);		goto ReGotoREGI; }			}						pStrcpy( thePrefs.NewPrefsCode, theStr);						thePrefs.NewPrefSystem = true;		}		else { Erreur( 29, -1);		goto ReGotoREGI; }				// Ecriture du code dans le fichier Prefs;				thePrefs.Registred = true;		thePrefs.SNumber = (long) theCode.Multiple;				theCode.Decompil = PrefsUpdate();				if( theCode.Decompil >= 255)		{			theCode.Decompil = 0;		}				if( theCode.Decompil == 0 && thePrefs.Registred == true)		{			DeletePreferences();						Erreur( 87, 87);						ExitToShell();		}				thePrefs.Decode = (Byte) theCode.Decompil;		for(i=0; i<32;i++) thePrefs.AutorName[ i] = UserName[i];		if( thePrefs.AutorName[ 0] > 30) thePrefs.AutorName[ 0] = 30;				WritePreferences();				ProtectInit();				Result = 0;	}		DisposDialog( TheDia);	SetPort( myPort);}/*void PubDialog(void){	long		Result;	short		itemHit, i;	DialogPtr	TheDia;	Str255		theStr, UserName;	GrafPtr		myPort;			GetPort( &myPort);	TheDia = GetNewDialog( 132,0L, (WindowPtr) -1L);	SetPortDialogPort( TheDia);	#ifdef DEMO	ControlSwitch( 3, TheDia, 255);	#endif	if( thePrefs.Registred)	{		ControlSwitch( 3, TheDia, 255);	}	ReGoto:	do	{		ModalDialog( MyDlgFilterDesc, &itemHit);		}while( itemHit != 1 && itemHit != 2 && itemHit != 3);		DisposDialog( TheDia);	SetPort( myPort);		switch( itemHit)	{		case 2:	// Print			PreparePrintRegistration();		break;				#ifndef DEMO		case 3:	// Registration			Registration();		break;		#endif	}}*/