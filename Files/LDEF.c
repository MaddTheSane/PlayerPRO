#include "Shuddup.h"
#include "GetFileIcon.h"

void MDrawTheText( Rect *bounds, Str255 data, Boolean selected, Str63 Str);
void MDrawItem( Boolean selected, Rect* bounds, Str255 data, Str63 Str, FSSpec*);
pascal	OSErr	DTGetIcon(ConstStr255Param volName,
						  short vRefNum,
						  short iconType,
						  OSType fileCreator,
						  OSType fileType,
						  Handle *iconHandle);
						  
extern	DialogPtr	MODListDlog;

void MDrawTheText( Rect *bounds, Str255 data, Boolean selected, Str63 Str)
{	
	if (selected)
	{
		Rect	aRect;
	
		aRect.left = bounds->left + 23;		// 38
		aRect.top = bounds->top + 4;
		aRect.right = aRect.left + StringWidth( data) + 2;
		aRect.bottom = aRect.top + 13;

		PaintRect( &aRect);

		ForeColor( whiteColor);
		BackColor( blackColor);
	}
	
	MoveTo( bounds->left + 24, bounds->top + 14);
	DrawString( data);
	
	if (selected)
	{
		ForeColor( blackColor);
		BackColor( whiteColor);
	}
}

void MDrawItem( Boolean selected, Rect* bounds, Str255 data, Str63 Str, FSSpec	*spec)
{
	PenState			ps;
	short			iconID;
	Boolean			doNewWay;
	IconTransformType	iconTransform;
	Rect				iconRect;
	Boolean			itemEnabled = true;
	Handle			gTheSuite;
	OSErr			iErr;
	FInfo				fndrInfo;
	
	iconRect.left		= bounds->left + 6;			// 21
	iconRect.top		= bounds->top + 2;
	iconRect.bottom	= iconRect.top + 16;
	iconRect.right		= iconRect.left + 16;
	
	if (!thePrefs.FastMusicList)
	{
		if (!selected) PlotIconID(&iconRect, atNone, ttNone, 133);
		else PlotIconID(&iconRect, atNone, ttSelected, 133);
	}
	else
	{
		IconRef	iconref;
		SInt16	label;
		
		iErr = GetIconRefFromFile( spec, &iconref, &label);
		
		if (iErr == noErr)
		{
			if (!selected) PlotIconRef( &iconRect, kAlignNone, kTransformNone, kIconServicesNormalUsageFlag, iconref);
			else PlotIconRef( &iconRect, kAlignNone, kTransformSelected, kIconServicesNormalUsageFlag, iconref);
			ReleaseIconRef( iconref);
		}
		else
		{
			iErr=GetIconRef( kOnSystemDisk, 'SNPL', 'MADK', &iconref);
			if (iErr == noErr)
			{
				if (!selected) PlotIconRef( &iconRect, kAlignNone, kTransformOffline, kIconServicesNormalUsageFlag, iconref);
				else PlotIconRef( &iconRect, kAlignNone, kTransformSelectedOffline, kIconServicesNormalUsageFlag, iconref);
				ReleaseIconRef( iconref);
			}
			else
			{
				if (!selected) PlotIconID(&iconRect, atNone, kTransformOffline, 188);
				else PlotIconID(&iconRect, atNone, kTransformSelectedOffline, 188);
			}
		}
	}
	MDrawTheText( bounds, data, selected, Str);
}
