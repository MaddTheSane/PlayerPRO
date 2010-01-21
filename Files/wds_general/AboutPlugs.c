/* About Plugs.c */

#include "Shuddup.h"
#include "RDriver.h"
#include "PrivateList.h"

short GetNoPPDG();

static PrivateList		myList;
static DialogPtr		aDialog;

#define POSINS		10

short	plugType[ 100];
short	plugID[ 100];

enum
{
	eMusicFilePlug,
	eSoundFilePlug,
	eSoundFilterPlug,
	eDigitalEditorPlug
};

void DrawPlugInfo( short iD)
{
	short		pos;
	Str255		aStr, tempStr;
	Rect		destRect;
	
	pos = iD - GetControlValue( myList.yScroll);
	
	destRect.top = myList.rect.top + myList.HCell*pos;
	destRect.bottom = destRect.top + myList.HCell;
	
	NNumToString( iD + 1, aStr);
	pStrcat( aStr, "\p  ");

	switch( plugType[iD])
	{
		case eMusicFilePlug:
			pStrcat( aStr, MADDriver->lib->ThePlug[ iD].file.name);
		break;
		
		case eSoundFilePlug:
		
			break;
		
		case eDigitalEditorPlug:
		
			break;
		
		case eSoundFilterPlug:
		
			break;
	}
	
	MoveTo( myList.rect.left + POSINS, myList.rect.top + 10 + myList.HCell*pos);
	DrawString( aStr);
	
	if( iD >= myList.select.top && iD <= myList.select.bottom)
	{
		Rect 			t;
		RGBColor		hilite;
	
		LMGetHiliteRGB( &hilite);
		
		t.left = myList.rect.left;						t.right = myList.rect.right;
		t.top = myList.rect.top + myList.HCell*pos;		t.bottom = t.top + myList.HCell-1;

		
		if( hilite.red == 0 &&
			hilite.green == 0 &&
			hilite.blue == 0)
		{
			InvertRect( &t);
		}
		else
		{
			RGBBackColor( &hilite);
			
			CopyBits ( 	(BitMap*) *GetPortPixMap(GetDialogPort( aDialog)),
						(BitMap*) *GetPortPixMap(GetDialogPort( aDialog)),
						&t,
						&t,
						srcCopy,
						nil);
			
			ForeColor( blackColor);
			BackColor( whiteColor);
		}
	}
}

void AboutPlugs()
{
	short			itemHit, itemType, i, x;
	Handle			itemHandle;
	Rect			itemRect;
	FontInfo		ThisFontInfo;
	
	aDialog = GetNewDialog( 154, NULL, (WindowPtr) -1L);
	SetPortDialogPort( aDialog);
	
	ChangeDialogFont( aDialog);
	
	///////// LIST CREATION ////////////
	
	GetDialogItem( aDialog, 2, &itemType, &itemHandle, &myList.rect);
	myList.rect.right -= 15;
	myList.rect.left = 0;
	
	SetRect( &itemRect, 0, 0, 30, 16);
	myList.yScroll = NewControl( 	GetDialogWindow( aDialog),
									&itemRect,
									"\p.",
									true,
									0,
									0,
									0,
									gScrollBarID,
									0);
							
	myList.xScroll = 0;
	myList.maxX = 1;
	myList.maxY = MADDriver->lib->TotalPlug + GetNoPPDG();
	myList.aDia = aDialog;
	myList.type = onlyOne;
	
	GetFontInfo( &ThisFontInfo);
	myList.HCell = ThisFontInfo.ascent + ThisFontInfo.descent + ThisFontInfo.leading + 1;
	myList.LCell = myList.rect.right - myList.rect.left;
	
	SetRect( &myList.select, 0, 0, 0, 0);
	
	PLSetControl( &myList);
	
	///////// LIST CREATION ////////////
	
	for( i = 0, x = 0; i < MADDriver->lib->TotalPlug; i++, x++)
	{
		plugType[ x]	= eMusicFilePlug;
		plugID[ x]		= i;
 	}
	
	for( i = 0, x = 0; i < GetNoPPDG(); i++, x++)
	{
		plugType[ x]	= eDigitalEditorPlug;
		plugID[ x]		= i;
	}
	
	do
	{
		ModalDialog( MyDlgFilterDesc, &itemHit);
		
		switch( itemHit)
		{
			case 2:
			{
				Point	pt;
				
				GetMouse( &pt);
			
				PLScroll( pt, &myList);
				
				PLClick( pt, 0, &myList);
				
			}
			break;
		}
		
	}while( itemHit != 1);
	
	DisposeDialog( aDialog);
}

void UpdatePlugsAbout( DialogPtr aDialog)
{
	RgnHandle	saveClipRgn;
	short		i;
	
	BeginUpdate( GetDialogWindow( aDialog));
	
	DrawDialog( aDialog);
	
	oldFrameButton( aDialog);
	
	saveClipRgn = NewRgn();
	GetClip( saveClipRgn);
	
	ClipRect( &myList.rect);
	
	ForeColor( whiteColor);
	PaintRect( &myList.rect);
	ForeColor( blackColor);
	
	for( i = GetControlValue( myList.yScroll); i < PLGetMaxYValue( &myList); i++)
	{
		DrawPlugInfo( i);
	}
	
	SetClip( saveClipRgn);
	DisposeRgn( saveClipRgn);
	
	InsetRect( &myList.rect, -1, -1);
	FrameRect( &myList.rect);
	InsetRect( &myList.rect, 1, 1);
	
	EndUpdate( GetDialogWindow( aDialog));
}
