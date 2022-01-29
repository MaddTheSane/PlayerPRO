#include "Packages.h"
#include "Printing.h"

void PrintPicture (PicHandle whichPic, Rect *whichDestRect);

		THPrint		hPrint;

void PrintPixMap( PixMapHandle aPix)
{
	GrafPtr		curPort;
	OSErr		err;
	Rect		destRect;

	GetPort( &curPort);

	CenterRect( &curPort->portRect, &(*aPix)->bounds, &destRect);

	ClipRect( &destRect);
	
	CopyBits(	(BitMap*) *( aPix),
 				&curPort->portBits,
 				&(*aPix)->bounds,
 				&destRect,
 				ditherCopy,				//ditherCopy
 				0L);
}

void PrintBitMap( BitMap aBit)
{
	GrafPtr		curPort;
	OSErr		err;
	Rect		destRect;

	GetPort( &curPort);

	CenterRect( &curPort->portRect, &aBit.bounds, &destRect);

	ClipRect( &destRect);
	
	CopyBits(	(BitMap*) &aBit,
 				&curPort->portBits,
 				&aBit.bounds,
 				&destRect,
 				ditherCopy,				//ditherCopy
 				0L);
}

void InitPrinting()
{
	hPrint = (THPrint) NewHandle( sizeof( TPrint) + 50L);
	
	PrOpen();
	
	PrintDefault( hPrint);
	
	PrClose();
}

void PageSetUp()
{
	PrOpen();
	
	PrStlDialog( hPrint);
	
	PrClose();
}

void DoneDrawingPagesOfATextFile( Ptr myText, long	TextLong)
{
	short	i;
	Str255	aStringOfText;

	TextSize( 9);
	TextFont( 4);
	
	for (i = 1; i <= TextLong/250; ++i)
	{
			BlockMoveData( myText, (Ptr) aStringOfText, 250);
			aStringOfText[ 250] = '\0';
			CtoPstr( (Ptr) aStringOfText);
			
			/*
			 * Carrige return characters mess up DrawString, so they are removed.
			 * Also, tabs are not handled correctly.  They are left as an exercise
			 * for the programmer!
			 */
			 
			if ( aStringOfText[ aStringOfText[0]] == '\n') aStringOfText[ aStringOfText[0]] = ' ';
			MoveTo(10, 14 * i);
			DrawString( aStringOfText);
			
			myText += 250;
	}
}
