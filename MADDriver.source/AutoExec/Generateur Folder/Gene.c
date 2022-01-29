#include "Packages.h"
#include "Printing.h"

extern THPrint		hPrint;

void pStrcpy(register unsigned char *s1, register unsigned char *s2);
void GenerateCodeNewID( Str255 strID, long num);
void GenerateCodeNewID2( Str255 strID, long num);
long NewPrefAnalyse();
void MyDebugStr( long, Ptr, Ptr);

OSErr TestNewCode2( Str255 str)
{
	if(	str[ 2] == 'A' &&
		str[ 3] == 'D' &&
		str[ 4] == '5') return noErr;
		
	else return -1;
}

long NewPrefAnalyse2( Str255 str)
{
	if(	TestNewCode2( str) == noErr)
	{
		Str32		tStr, Div19;
		long		i;
		long 		tt, ttc;
		short		microVal;
		
		for( i = 0; i < 4; i++) tStr[ i] = str[ 5 + i*2];		// UNE LETTRE SUR 2 !!
		for( i = 0; i < 6; i++) Div19[ i] = str[ 6 + i*2];	// UNE LETTRE SUR 2 !!
		Div19[ 5] = str[ 6 + 9];
		
		Div19[ 0] = (Div19[ 0] + 0x30) - 0x51;
		Div19[ 1] = (Div19[ 1] + 0x30) - 0x50;
		Div19[ 2] = (Div19[ 2] + 0x30) - 0x51;
		Div19[ 3] = (Div19[ 3] + 0x30) - 0x50;
		Div19[ 4] = (Div19[ 4] + 0x30) - 0x51;
		Div19[ 5] = (Div19[ 5] + 0x30) - 0x51;
		Div19[ 6] = 0;
		
		tt = ttc = atol( (Ptr) Div19);
		
		tt /= 17;			// Multiple de 13 !
		
		tt *= 17;
		
		if( tt != ttc) return 0;
		
		microVal = tt & 0x0000000F;
		
		tStr[ 0] = (tStr[ 0] + 0x30) - 0x50 + microVal;	// Lettre, a partir de 'P'
		tStr[ 1] = (tStr[ 1] + 0x30) - 0x51 + microVal;	// Lettre
		tStr[ 2] = (tStr[ 2] + 0x30) - 0x50 + microVal;	// Numero
		tStr[ 3] = (tStr[ 3] + 0x30) - 0x51 + microVal;	// Lettre, a partir de 'U'
		
		tStr[ 4] = 0;
		
		tt = atol( (Ptr) tStr);
		
		tt /= 0x21;		// --> 0x38
		
		return tt;
	}
	
	return 0;
}
void GenerateCodeNewID2( Str255 strID, long num)
{
	Str32	tStr;
	Str32	Div19;
	long	microVal, i, tt;
	
	pStrcpy( strID, "\pCAD5TTTTTTTTTTTTTTTTTTTTT");
	
	strID[ 0] = 4 + 6 + 5;
	
	// -------------
	
	tt = num * 17;
	
//	pStrcpy( Div19, "\pHHHHHHHHHHHHHHHHHHHH");
//	NumToString( tt, Div19);
	
	sprintf( (Ptr) Div19, "%6ld", tt);
	for( i = 0; i < 6; i++) if( Div19[ i] == ' ') Div19[ i] = '0';
	
	Div19[ 0] = (Div19[ 0] - 0x30) + 0x51;
	Div19[ 1] = (Div19[ 1] - 0x30) + 0x50;
	Div19[ 2] = (Div19[ 2] - 0x30) + 0x51;
	Div19[ 3] = (Div19[ 3] - 0x30) + 0x50;
	Div19[ 4] = (Div19[ 4] - 0x30) + 0x51;
	Div19[ 5] = (Div19[ 5] - 0x30) + 0x51;
	
	microVal = tt & 0x0000000F;
	
	// -------------
	
	tt = 0x38;
	tt *= 0x21;
	
	NumToString( tt, tStr);
	
	tStr[ 0] = (tStr[ 1] - 0x30) + 0x50 - microVal;
	tStr[ 1] = (tStr[ 2] - 0x30) + 0x51 - microVal;
	tStr[ 2] = (tStr[ 3] - 0x30) + 0x50 - microVal;
	tStr[ 3] = (tStr[ 4] - 0x30) + 0x51 - microVal;
	
	for( i = 0; i < 4; i++) strID[ 5 + i*2] = tStr[ i];		// UNE LETTRE SUR 2 !!
	for( i = 0; i < 6; i++) strID[ 6 + i*2] = Div19[ i];	// UNE LETTRE SUR 2 !!
	strID[ 6 + 9] = Div19[ 5];
	
	if( NewPrefAnalyse2( strID) != 0x38) {SysBeep( 1); MyDebugStr( __LINE__, __FILE__, "");}
}

unsigned char *pStrcat(unsigned char *s, unsigned char *t)
{
	unsigned char *s2;
	short tLen;

	s2 = s + *s;
	*s += (tLen = *t);
	for (++tLen; --tLen; s2[tLen] = t[tLen]);
	return (s);
}

void CenterRect( Rect *main, Rect *dst, Rect *result)
{
	Rect		tempRect;
	short			Larg, Haut;
	
	tempRect.left = main->left;		tempRect.right = main->right;
	tempRect.top = main->top;		tempRect.bottom = main->bottom;
	
	Larg = dst->right - dst->left;
	Haut = dst->bottom - dst->top;
	
	result->left = tempRect.left + - Larg/2 + (tempRect.right - tempRect.left)/2;
	result->right = result->left + Larg;
	
	result->top = tempRect.top + - Haut/2 + (tempRect.bottom - tempRect.top)/2;
	result->bottom = result->top + Haut;
}

void pStrcpy(register unsigned char *s1, register unsigned char *s2)
{
	register short len, i;
	
	if (*s2 <= 50) 
	{
		len = *s2;
		for ( i = 0; i <= len; i++)
		{
			s1[ i] = s2[ i];
		}
	}
	else Debugger();
}


void SetDText (DialogPtr dlog, short item, Str255 str)
{
Handle	itemHandle;
short	itemType;
Rect	itemRect;

	GetDialogItem (dlog, item, &itemType, &itemHandle, &itemRect);
	if( itemHandle == 0L) DebugStr("\pError in SetDText");
	SetDialogItemText (itemHandle, str);
}

void GetDText (DialogPtr dlog, short item, StringPtr str)
{
Handle	itemHandle;
short	itemType;
Rect	itemRect;

	GetDialogItem (dlog, item, &itemType, &itemHandle, &itemRect);
	if( itemHandle == 0L) DebugStr("\pError in SetDText");
	GetDialogItemText (itemHandle, str);
}

void StartupDialog( long *code, long *page)
{
	DialogPtr	aDialog;
	short		itemHit, i;
	Str255		aStr;
	Handle		aRsrc;
	
	aDialog = GetNewDialog( 3214, 0L, (WindowPtr) -1L);
	SetPort( aDialog);
	
	NumToString( *code, aStr);
	SetDText( aDialog, 6, aStr);
	
	do
	{
		ModalDialog( 0L, &itemHit);
		
	}while( itemHit != 1 && itemHit != 2);
		
	
	GetDText( aDialog, 6, aStr);
	StringToNum( aStr, code);
	
	GetDText( aDialog, 4, aStr);
	StringToNum( aStr, page);
	
	DisposeDialog( aDialog);

	
	if( itemHit == 1) return;
	else ExitToShell();
}

GrafPtr	thePort;

void MyDebugStr( long, Ptr, Ptr)
{

}

void main( void)
{
	short	fRefNum;
	long	inOutCount, *tempL, page, code, i;
	short		*CODE;
	Ptr		bufPtr;
	Handle	aRsrc;
	Str255	tempStr, finalStr;
	WindowPtr	aPort, theFrontWindow;
	Point		theCell;
	short		WindType;
	GrafPtr		savePort, curPort;
	TPrStatus	prStatus;
	TPPrPort	printPort;
	OSErr		err;
	PicHandle	aPict;
	Rect		destRect;

	InitGraf( &qd.thePort);
	InitFonts();
	FlushEvents(everyEvent,0);
	InitWindows();
	TEInit();
	InitMenus();
	InitCursor();
	MaxApplZone();

/*
	NumŽro de code:

	0xCDEF GHIJ
	
	1: Multiple de 17
	2: Code de dŽcompilation 0x38
*/
 
//	#define LISTING
 	#define NEWCODE
 
 	#ifdef LISTING
 	{
 		short		fRefNum;
 		long		inOutCount;
 	
 	#define NAME	"\pList Code PlayerPRO"
 	
	FSDelete( NAME, 0);
 	Create( NAME, 0, 'ttxt', 'TEXT');
 	FSOpen( NAME, 0,&fRefNum);
 	
 	CODE = (short*) NewPtrClear( 4);
	tempL = (long*) CODE;
 	
 	for( i = 900; i < 950; i++)
 	{
 		#ifdef NEWCODE
 			GenerateCodeNewID2( tempStr, i);
 			pStrcpy( finalStr, tempStr);
 		#else
	 		CODE[ 0] = 0x0011*i;
	 		CODE[ 1] = 0x0038;
			
	 		NumToString( *tempL, tempStr);
			
	 		if( tempStr[0] == 7) pStrcpy( finalStr, "\p00");
	 		else if( tempStr[0] == 8) pStrcpy( finalStr, "\p0");
	 		else pStrcpy( finalStr, "\p");
	 		pStrcat( finalStr, tempStr);
 		#endif
 		
 		pStrcat( finalStr, "\p\r");
 		
 		inOutCount = finalStr[ 0];
 		FSWrite( fRefNum, &inOutCount, finalStr+1);
 	}
 	
 	FSClose( fRefNum);
 	
 	ExitToShell();
 	}
 	#endif
 
 
 
 
 
 	InitPrinting();

	aRsrc = GetResource( 'CODD', 128);
	if( aRsrc == 0L) Debugger();
	code = *((long*) *aRsrc);

	StartupDialog( &code, &page);
	
	code += page*4;
	*((long*) *aRsrc) = code;
	ChangedResource( aRsrc);
	WriteResource( aRsrc);

	page *= 4;

	PageSetUp();

	aPict = GetPicture( 128);

	PrOpen();
	
	if( PrJobDialog( hPrint))
	{
		GetPort( &savePort);

		printPort = PrOpenDoc( hPrint, nil, nil);
		SetPort( &printPort->gPort);


		CODE = (short*) NewPtrClear( 4);
 		tempL = (long*) CODE;
 	
 		for( i = code; i < code + page;)
 		{
			PrOpenPage( printPort, nil);		// Open this page ...
		
			GetPort( &curPort);
		
			CenterRect( &curPort->portRect, &(*aPict)->picFrame, &destRect);
			DrawPicture( aPict, &destRect);
			
			#ifdef NEWCODE
 				TextFont( 20);
				TextSize( 20);
 			#else
 				TextFont( 21);
				TextSize( 30);
			#endif
			
	 		/*** 1re ligne ***/
	 		
	 		#ifdef NEWCODE
 				GenerateCodeNewID2( tempStr, i);
 				pStrcpy( finalStr, tempStr);
 			#else
	 			CODE[ 0] = 0x0011*i;
	 			CODE[ 1] = 0x0038;
				
		 		NumToString( *tempL, tempStr);
				
		 		if( tempStr[0] == 7) pStrcpy( finalStr, "\p00");
		 		else if( tempStr[0] == 8) pStrcpy( finalStr, "\p0");
		 		else pStrcpy( finalStr, "\p");
		 		pStrcat( finalStr, tempStr);
			#endif
			
	 		
			MoveTo( destRect.right/2 - 55 + 134 + 25, destRect.top + 167 - 53);
			DrawString( finalStr);
	
			/*** 2eme ligne ***/
			i++;
	 		#ifdef NEWCODE
 				GenerateCodeNewID2( tempStr, i);
 				pStrcpy( finalStr, tempStr);
 			#else
	 			CODE[ 0] = 0x0011*i;
	 			CODE[ 1] = 0x0038;
				
		 		NumToString( *tempL, tempStr);
				
		 		if( tempStr[0] == 7) pStrcpy( finalStr, "\p00");
		 		else if( tempStr[0] == 8) pStrcpy( finalStr, "\p0");
		 		else pStrcpy( finalStr, "\p");
		 		pStrcat( finalStr, tempStr);
			#endif
			
			MoveTo( destRect.right/2 - 55 + 134 + 25, destRect.top + 525 - 140);
			DrawString( finalStr);
			/*********************/
			
			/*** 3eme ligne ***/
			i++;
	 		#ifdef NEWCODE
 				GenerateCodeNewID2( tempStr, i);
 				pStrcpy( finalStr, tempStr);
 			#else
	 			CODE[ 0] = 0x0011*i;
	 			CODE[ 1] = 0x0038;
				
		 		NumToString( *tempL, tempStr);
				
		 		if( tempStr[0] == 7) pStrcpy( finalStr, "\p00");
		 		else if( tempStr[0] == 8) pStrcpy( finalStr, "\p0");
		 		else pStrcpy( finalStr, "\p");
		 		pStrcat( finalStr, tempStr);
			#endif
			
			MoveTo( destRect.right/2 - 55 - 220 + 25, destRect.top + 525 - 140);
			DrawString( finalStr);
			/*********************/

			/*** 4eme ligne ***/
			i++;
	 		#ifdef NEWCODE
 				GenerateCodeNewID2( tempStr, i);
 				pStrcpy( finalStr, tempStr);
 			#else
	 			CODE[ 0] = 0x0011*i;
	 			CODE[ 1] = 0x0038;
				
		 		NumToString( *tempL, tempStr);
				
		 		if( tempStr[0] == 7) pStrcpy( finalStr, "\p00");
		 		else if( tempStr[0] == 8) pStrcpy( finalStr, "\p0");
		 		else pStrcpy( finalStr, "\p");
		 		pStrcat( finalStr, tempStr);
			#endif
			MoveTo( destRect.right/2 - 55 - 220 + 25, destRect.top + 167 - 53);
			DrawString( finalStr);
			/*********************/

			i++;

			PrClosePage( printPort);          	// Close this page ...
		}
		PrCloseDoc( printPort);
	
		// Handle print spooler
	
		if (((*hPrint)->prJob.bJDocLoop = bSpoolLoop) && (!PrError() ) )
					PrPicFile(hPrint, nil, nil, nil, &prStatus);

		SetPort(savePort);
	}
	PrClose();
}
