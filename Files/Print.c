#include "Shuddup.h"
#include <Carbon/Carbon.h>
//FIXME: Make this work with OS X.  Currently non-OS X compatible bits are commented out.

void		PrintPicture (PicHandle whichPic, Rect *whichDestRect);
TEHandle	GetTEHelp();
void		PrintEditor();

extern	WindowPtr	oldWindow;

//static	THPrint		hPrint;
//static	TPPrPort	printPort;

		Str255		versString;

void DrawMODHeader(void)
{
	Str255				aStr, str2;
	unsigned long		secs;
	Handle				aHandle;
	
	TextFont(4);	TextSize(9);
	MoveTo(20, 20);
	
	DrawString(versString);
	DrawString("\p   ");
	
	pStrcpy(aStr, "\pFilename: ");
	pStrcat(aStr, curMusic->musicFileName);
	DrawString(aStr);
	
	DrawString("\p   ");
	DrawString("\pPrinting: ");
	GetDateTime(&secs);
	DateString(secs, longDate, aStr, NULL);
	DrawString(aStr);
	DrawString("\p at ");
	TimeString(secs, longDate, aStr, NULL);
	DrawString(aStr);
}

void PrintPixMap(PixMapHandle aPix)
{
	GrafPtr		curPort;
	OSErr		err;
	Rect		caRect, destRect;

	if (aPix == NULL) return;
	
	GetPort(&curPort);
	
	GetPortBounds((CGrafPtr) curPort, &caRect);
	
	CenterRect(&caRect, &(*aPix)->bounds, &destRect);

	ClipRect(&destRect);
	
	CopyBits(	(BitMap*) *(aPix),
 				(BitMap*) *GetPortPixMap((CGrafPtr) curPort),
 				&(*aPix)->bounds,
 				&destRect,
 				ditherCopy,				//ditherCopy
 				NULL);
}

void PrintBitMap(BitMap aBit)
{
	GrafPtr		curPort;
	OSErr		err;
	Rect		caRect, destRect;

	GetPort(&curPort);
	
	GetPortBounds( (CGrafPtr) curPort, &caRect);
	
	CenterRect(&caRect, &aBit.bounds, &destRect);

	ClipRect(&destRect);
	
	CopyBits(	(BitMap*) &aBit,
 				(BitMap*) *GetPortPixMap( (CGrafPtr) curPort),
 				&aBit.bounds,
 				&destRect,
 				ditherCopy,				//ditherCopy
 				NULL);
}

Boolean PageSetUp(void)
{
	Boolean	val = false;

#if MACOS9VERSION

	PrOpen();
	
	val = PrStlDialog(hPrint);
	
	PrClose();
	
#endif
	
	return val;
}

void InitPrinting(void)
{
#if MACOS9VERSION
	
	hPrint = (THPrint) MyNewHandle(sizeof(TPrint) + 50L);
	
	PrOpen();
	
	PrintDefault(hPrint);
	
	PrClose();
	
#endif
}

void Print(void)
{
	WindowPtr	aPort, theFrontWindow;
	Point		theCell;
	short		WindType;
	GrafPtr		savePort;
//	TPrStatus	prStatus;
	OSErr		err;
	Rect		destRect;
	
	WindType = GetWRefCon(oldWindow);
	
	switch(WindType)
	{
		case RefPartition:
		case RefSample:
		case RefHelp:
		case 0:
				
		break;
	
		default:
			Erreur(34, -4);
			return;
		break;
	}
	
#if MACOS9VERSION
	
	PrOpen();
	
	if (PrJobDialog(hPrint))
	{
		printPort = PrOpenDoc(hPrint, nil, nil);
		if (PrError() == noErr)
		{			
			switch(WindType)
			{
				case 0:
					PrOpenPage(printPort, nil);
					DrawMODHeader();

					PrintRegistration();
					
					PrClosePage(printPort);
				break;
			
				case RefSample:
					PrOpenPage(printPort, nil);
					DrawMODHeader();
					PrintSample(oldWindow);
					PrClosePage(printPort);
				break;
				
				case RefPartition:
					PrintEditor();
				break;
				
				case RefHelp:
					PrintHelpOnline();
				break;
				
				default:
					Erreur(34, -4);
					return;
				break;
			}
			if ((*hPrint)->prJob.bJDocLoop == bSpoolLoop  && PrError() == noErr )
				PrPicFile(hPrint, NULL, NULL, NULL, &prStatus );

			PrCloseDoc(printPort);
		}
		else Erreur(54, PrError());

		SetCursor(GetQDGlobalsArrow(&qdarrow));
	}
	PrClose();
	
#endif
}


void DoneDrawingPagesOfATextFile(Ptr myText, long	TextLong)
{
	short	i;
	Str255	aStringOfText;

	TextSize(9);
	TextFont(4);
	
	for (i = 1; i <= TextLong/250; ++i)
	{
			BlockMoveData(myText, (Ptr) aStringOfText, 250);
			aStringOfText[ 250] = '\0';
			MyC2PStr((Ptr) aStringOfText);
			
			/*
			 * Carrige return characters mess up DrawString, so they are removed.
			 * Also, tabs are not handled correctly.  They are left as an exercise
			 * for the programmer!
			 */
			 
			if (aStringOfText[ aStringOfText[0]] == '\n') aStringOfText[ aStringOfText[0]] = ' ';
			MoveTo(10, 14 * i);
			DrawString(aStringOfText);
			
			myText += 250;
	}
}

void PrintTEHandle(TEHandle hTE)
{
#define Margins 20

	short 		totalLines;			//{ number of lines in text }
	Rect		rView;				//{ viewRect for TERect }
	GrafPtr		oldPort;			//{ hold original grafPtr }
	Rect		oldView;			//{ hold original viewRect }
	Rect		oldDest;			//{ hold original destRect }
	short		totalHeight;		//{ lineHeight for TERec }
	short 		currentLine;		//{ what line are we on }
	short		scrollAmount;		//{ how much we scroll by }
	Rect		zeroRect;			//{ 0,0,0,0 rect used in clipRect }
	short		viewHeight;			//{ temp that has the viewRect height+1 to test conditions }	
	Boolean		OpenPrintManager;	//{ flag if print manager can be opened okay }
	Boolean		abort;				//{ flag if cmd-period is hit to exit routine }

#if MACOS9VERSION

	oldView = (*hTE)->viewRect; 
	oldDest = (*hTE)->destRect;
	oldPort	= (*hTE)->inPort;
	
	SetPort((GrafPtr) printPort);
	SetRect(&zeroRect, 0, 0, 0, 0 );
	
	rView = (*hPrint)->prInfo.rPage;
	InsetRect(&rView, Margins, Margins );
	(*hTE)->inPort = (GrafPtr) printPort;
	(*hTE)->destRect = rView;
	(*hTE)->viewRect = rView;
	TECalText(hTE );
	totalLines = (*hTE)->nLines;
	totalHeight = TEGetHeight(totalLines, 0, hTE );
	(*hTE)->destRect.bottom = (*hTE)->destRect.top + totalHeight;
	
	abort = false;
	currentLine = 1;
	
	while (abort != true && currentLine <= totalLines)
	{
		PrOpenPage(printPort, NULL );
		scrollAmount = 0;
		ClipRect(&(*hPrint)->prInfo.rPage );
		
		viewHeight = (*hTE)->viewRect.bottom - (*hTE)->viewRect.top + 1;

		//	{ figure out how many lines there are per page }
	
		while (((scrollAmount + TEGetHeight(currentLine, currentLine, hTE ) ) <= viewHeight )
			&& (currentLine <= totalLines ) )
			{
				scrollAmount = scrollAmount + TEGetHeight(currentLine, currentLine, hTE );
				currentLine = currentLine + 1;
			}
		
		(*hTE)->viewRect.bottom = scrollAmount + Margins;	//{ Add margins since top has a margin }
		TEDeactivate(hTE ); 								//{ Deactive the edit record so we don't print the cursor or selection range }
		TEUpdate(&(*hTE)->viewRect, hTE ); 				//{ print the page }
		ClipRect(&zeroRect); 								//{ Close clipping so that TEScroll doesn't redraw the text }
		TEScroll(0, -scrollAmount, hTE ); 					//{ scroll the page so we can print the next one }
		(*hTE)->viewRect.bottom = rView.bottom; 			//{ reset bottom to full page }

		if (PrError() == iPrAbort) abort = true;
		PrClosePage(printPort ); 						//{ close everything up }
	}
	
	(*hTE)->inPort = oldPort;
	(*hTE)->viewRect = oldView;
	(*hTE)->destRect = oldDest;
	TECalText(hTE);
	
#endif
}
