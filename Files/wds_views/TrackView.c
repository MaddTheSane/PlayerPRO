#include "Shuddup.h"
#include "MAD.h"
#include "RDriver.h"

#define TrackListSize 	32
#define	ILARG			13


		DialogPtr				TrackViewDlog;

extern	Boolean					MicroPhone;

static	long					oldTrackTicks;
static	Rect					TracksBox[ TrackListSize];
static	short 					oldTube[ TrackListSize];

void SmallEffectTrack(void)
{
	short	i;
	
	for (i = 0; i < 16 ; i++)
	{
		MADDriver->Tube[i] = i*2 + 32;
	}
	
	for (i = 0; i < 16 ; i++)
	{
		MADDriver->Tube[i + 16] = 64 - i*2;
	}
}

void DoGrowTrackView(void)
{
	short		temp;
	Rect		caRect;

	GetPortBounds(GetDialogPort(TrackViewDlog), &caRect);
	
	if (caRect.bottom >= 136) temp = 69;
	else temp = 136;
	
	MySizeWindow(TrackViewDlog, 212, temp, true);
	
	SmallEffectTrack();
}

void DoNullTrackView(void)
{
	GrafPtr		SavePort;
	short		i, max;
	Rect		caRect, itemRect;

 	if (TrackViewDlog == NULL) return;
	if (TickCount() <= oldTrackTicks) return;
	oldTrackTicks = TickCount();
	
 	GetPort(&SavePort);
 	SetPortDialogPort(TrackViewDlog);
	
	GetPortBounds(GetDialogPort(TrackViewDlog), &caRect);

	if (caRect.bottom >= 136) max = 32;
	else max = 16;
	
	for (i = 0; i < max; i++)
	{
		if (MADDriver->Tube[i] >= 0)
		{
			itemRect = TracksBox[i];
		
			if (MADDriver->Tube[ i] == oldTube[ i])
			{
				itemRect.bottom -= MADDriver->Tube[i];
				MADDriver->Tube[i]--;
				oldTube[ i] = MADDriver->Tube[i];
				
				if (oldTube[i] >= 0)
				{
					MoveTo(itemRect.left, itemRect.bottom);
					LineTo(itemRect.right-1, itemRect.bottom);
				}
			}
			else
			{
				if (MicroPhone)
				{
					ForeColor(blueColor);
				}
				else
				{
					SwitchColor(i);
				}
				if (MADDriver->Tube[i] <= 0) MADDriver->Tube[i] = 1;
				
				itemRect.top = itemRect.bottom - MADDriver->Tube[i]+1;
				PaintRect(&itemRect);
				ForeColor(blackColor);
				
				if (MADDriver->Tube[ i] != 64)
				{
					itemRect.bottom = itemRect.top;
					itemRect.top = TracksBox[i].top;
					PaintRect(&itemRect);
				}
				
				oldTube[ i] = MADDriver->Tube[i];
			}
		}
	}

	SetPort(SavePort);
}

void  UpdateTrackView(DialogPtr GetSelection)  	/* Pointer to this dialog */
{ 
		GrafPtr		SavePort;
 		Handle		itemHandle;
		short		itemType, i, max;
		Rect		caRect, itemRect;
		Str255		stemp;

 		GetPort(&SavePort);
 		SetPortDialogPort(TrackViewDlog);

 		TextFont(4);
 		TextSize(9);

		BeginUpdate(GetDialogWindow(TrackViewDlog));
		
		GetPortBounds(GetDialogPort(TrackViewDlog), &caRect);

		if (caRect.bottom >= 136) max = 32;
		else max = 16;

		GetDialogItem (TrackViewDlog, 1, &itemType, &itemHandle, &itemRect);
		InsetRect(&itemRect, -1, -1);
		itemRect.bottom--;		FrameRectRelief(&itemRect);
		GetDialogItem (TrackViewDlog, 2, &itemType, &itemHandle, &itemRect);
		InsetRect(&itemRect, -1, -1);
		itemRect.bottom--;		FrameRectRelief(&itemRect);

 		for (i = 0; i < max; i++)
		{
			itemRect = TracksBox[ i];
			itemRect.top--;	itemRect.left--;	
			PaintRect(&itemRect);
			itemRect.top++;	itemRect.left++;
			
			if (MicroPhone) ForeColor(blueColor);
			else
			{
				SwitchColor(i);
			}
			if (MADDriver->Tube[i] > 80) MADDriver->Tube[i] = 80;
			itemRect.top = itemRect.bottom - MADDriver->Tube[i]+1;
			PaintRect(&itemRect);
			ForeColor(blackColor);
						
		/*	NTStr(2, i+1, (Ptr) stemp);
			MoveTo(itemRect.left + 5, itemRect.bottom + 9);
			MyC2PStr((Ptr) stemp);
			DrawString(stemp);*/
		}
		
		EndUpdate(GetDialogWindow(TrackViewDlog));

		TextFont(0);
		TextSize(0);

		SetPort(SavePort);
} 

void CreateTrackView(void)
{
	Rect		itemRect;
	Handle		itemHandle;
	short		itemType, i;

	SetItemMark(ViewsMenu, 5, checkMark);

	TrackViewDlog = GetNewDialog(157, NULL, GetDialogWindow(ToolsDlog));
	SetWindEtat(GetDialogWindow(TrackViewDlog));
	ShowWindow(GetDialogWindow(TrackViewDlog));
	SetPortDialogPort(TrackViewDlog);
	SelectWindow2(GetDialogWindow(TrackViewDlog));
	
	GetDialogItem (TrackViewDlog, 3, &itemType, &itemHandle, &itemRect);
	itemRect.top++;	itemRect.right--;	itemRect.left++;
	for (i = 0; i < 16; i++)
	{
		TracksBox[i] = itemRect;
		
		itemRect.left += ILARG;
		itemRect.right += ILARG;
	}
	GetDialogItem (TrackViewDlog, 4, &itemType, &itemHandle, &itemRect);
	itemRect.top++;	itemRect.right--;	itemRect.left++;
	for (i = 16; i < 32; i++)
	{
		TracksBox[i] = itemRect;
		
		itemRect.left += ILARG;
		itemRect.right += ILARG;
	}

	
	oldTrackTicks = 0;
}

void CloseTrackViewDlog(void)
{
	if (TrackViewDlog != NULL) DisposeDialog(TrackViewDlog);
	TrackViewDlog = NULL;

	SetItemMark(ViewsMenu, 5, noMark);
}
