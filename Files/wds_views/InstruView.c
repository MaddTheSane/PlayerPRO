#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"

		DialogPtr				InstruViewDlog;

static	long					oldInstruTicks;
static	Rect					InstruBox[ MAXINSTRU];

#define WSMALL		69
#define WMEDIUM		136
#define WLARG		270
#define WLARG2		538

#define	ILARG			13

void DoNullInstruView();

void SmallEffect(void)
{
	short	i;
	
	for( i = 0; i < MAXINSTRU ; i++)
	{
		MADDriver->InstruTube[ i] = 64;
		MADDriver->InstruActif[ i] = 1;
	}
}

void DoGrowInstruView(void)
{
short		temp;
Rect		caRect;

	GetPortBounds( GetDialogPort( InstruViewDlog), &caRect);
	
	if( caRect.bottom >= WLARG2) temp = WSMALL;
	else if( caRect.bottom >= WLARG) temp = WLARG2;
	else if( caRect.bottom >= WMEDIUM) temp = WLARG;
	else temp = WMEDIUM;
	
	MySizeWindow( InstruViewDlog, 212, temp, true);		//caRect.right
	
	SmallEffect();
}

void DoNullInstruView(void)
{
	Str255		String;
	GrafPtr		SavePort;
	short		i, max;
 	Rect		caRect,tempRect;
	Handle		itemHandle;
	short		itemType;
	Rect		itemRect;

 	if( InstruViewDlog == NULL) return;
	if( TickCount() <= oldInstruTicks) return;
	oldInstruTicks = TickCount();
	
 	GetPort( &SavePort);
 	SetPortDialogPort( InstruViewDlog);
	
	GetPortBounds( GetDialogPort( InstruViewDlog), &caRect);
	
	if( caRect.bottom >= WLARG2) max = 128;
	else if( caRect.bottom >= WLARG) max = 64;
	else if( caRect.bottom >= WMEDIUM) max = 32;
	else max = 16;
	
	for( i = 0; i < max; i++)
	{
		if( MADDriver->InstruTube[i] > 0)
		{
			itemRect = InstruBox[i];
			
			if( MADDriver->InstruTube[i] == 64)
			{
				MADDriver->InstruTube[i]--;
				SwitchColor( MADDriver->InstruActif[ i]);
				
				itemRect.top = itemRect.bottom - MADDriver->InstruTube[i];
				PaintRect( &itemRect);
				ForeColor( blackColor);
			}
			else
			{
				itemRect.bottom -= MADDriver->InstruTube[i];
				MADDriver->InstruTube[i]--;
				
				MoveTo( itemRect.left, itemRect.bottom);
				LineTo( itemRect.right-1, itemRect.bottom);
			}
		}
	}

	SetPort( SavePort);
}

void  UpdateInstruView(DialogPtr GetSelection)  	/* Pointer to this dialog */
{ 
		GrafPtr		SavePort;
 		Handle		itemHandle;
		short		itemType, i, max;
		Rect		caRect, itemRect;
		Str255		stemp;

 		GetPort( &SavePort);
 		SetPortDialogPort( InstruViewDlog);

 		TextFont( 4);
 		TextSize( 9);

		BeginUpdate( GetDialogWindow( InstruViewDlog));
	
		GetDialogItem (InstruViewDlog, 1, &itemType, &itemHandle, &itemRect);
		InsetRect( &itemRect, -1, -1);
		itemRect.bottom--;		FrameRectRelief( &itemRect);
		GetDialogItem (InstruViewDlog, 7, &itemType, &itemHandle, &itemRect);
		InsetRect( &itemRect, -1, -1);
		itemRect.bottom--;		FrameRectRelief( &itemRect);
		GetDialogItem (InstruViewDlog, 8, &itemType, &itemHandle, &itemRect);
		InsetRect( &itemRect, -1, -1);
		itemRect.bottom--;		FrameRectRelief( &itemRect);
		GetDialogItem (InstruViewDlog, 6, &itemType, &itemHandle, &itemRect);
		InsetRect( &itemRect, -1, -1);
		itemRect.bottom--;		FrameRectRelief( &itemRect);
		GetDialogItem (InstruViewDlog, 14, &itemType, &itemHandle, &itemRect);
		InsetRect( &itemRect, -1, -1);
		itemRect.bottom--;		FrameRectRelief( &itemRect);
		GetDialogItem (InstruViewDlog, 15, &itemType, &itemHandle, &itemRect);
		InsetRect( &itemRect, -1, -1);
		itemRect.bottom--;		FrameRectRelief( &itemRect);
		GetDialogItem (InstruViewDlog, 16, &itemType, &itemHandle, &itemRect);
		InsetRect( &itemRect, -1, -1);
		itemRect.bottom--;		FrameRectRelief( &itemRect);
		
		GetPortBounds( GetDialogPort( InstruViewDlog), &caRect);
		
		if( caRect.bottom >= WLARG2) max = 128;
		else if( caRect.bottom >= WLARG) max = 64;
		else if( caRect.bottom >= WMEDIUM) max = 32;
		else max = 16;
		
		for( i = 0; i < max; i++)
		{
			itemRect = InstruBox[ i];
			itemRect.top--;	itemRect.left--;	
			PaintRect( &itemRect);
			itemRect.top++;	itemRect.left++;
			
			SwitchColor( MADDriver->InstruActif[ i]);
			
			itemRect.top = itemRect.bottom - MADDriver->InstruTube[i]+1;
			PaintRect( &itemRect);
			ForeColor( blackColor);
	
			itemRect.right += ILARG;
			itemRect.left += ILARG;
		}
		
		EndUpdate( GetDialogWindow( InstruViewDlog));

		SetPort( SavePort);
} 

void CreateInstruView(void)
{
	Rect		itemRect, tempRect, dataBounds;
	Handle		itemHandle;
	short		itemType, itemHit, temp, i;
	Point		cSize;
	FontInfo	ThisFontInfo;
	Str255		String;

	if( InstruViewDlog != NULL)
	{
		SetWindEtat( GetDialogWindow(InstruViewDlog));
		return;
	}

	SetItemMark( ViewsMenu, mInsV, checkMark);

	InstruViewDlog = GetNewDialog( 129, NULL, GetDialogWindow( ToolsDlog));
	SetWindEtat( GetDialogWindow(InstruViewDlog));
	ShowWindow( GetDialogWindow( InstruViewDlog));
	SetPortDialogPort( InstruViewDlog);
	SelectWindow2( GetDialogWindow( InstruViewDlog));
	
	GetDialogItem (InstruViewDlog, 2, &itemType, &itemHandle, &itemRect);
	itemRect.top++;	itemRect.right--;	itemRect.left++;
	for( i = 0; i < 128; i++)
	{
		InstruBox[ i] = itemRect;
		
		itemRect.left += ILARG;
		itemRect.right += ILARG;
		
		if( (i+1) % 16 == 0)
		{
			short high;
			
			high = itemRect.bottom - itemRect.top;
			
			itemRect.bottom += high + 4;
			itemRect.top += high + 4;
			
			itemRect.left -= ILARG * 16;
			itemRect.right -= ILARG * 16;
		}
	}
	oldInstruTicks = 0;
}

void CloseInstruViewDlog(void)
{
	if( InstruViewDlog != NULL) DisposeDialog( InstruViewDlog);
	InstruViewDlog = NULL;
	
	SetItemMark( ViewsMenu, mInsV, noMark);
}
