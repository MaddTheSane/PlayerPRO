#ifndef __PRIVATELISTH__
#define __PRIVATELISTH__
#include <Carbon/Carbon.h>

enum
{
	yScrollNum,
	xScrollNum
};

enum
{
	onlyOne = 1,
	dragItem = 2
};

struct PrivateList
{
	Rect			rect;			// display rect
	Rect			select;		// selection rect
	short			LCell;
	short			HCell;
	ControlHandle	xScroll;
	ControlHandle	yScroll;
	
	short		maxY;
	short		maxX;
	
	DialogPtr		aDia;
	
	short		type;
	
	short		thumb;
};
typedef struct PrivateList PrivateList;

/** Functions **/

short PLGetCtlValue( ControlHandle	aH);
void PLSetCtlValue( ControlHandle	aH, short val);
short PLGetMaxYValue( PrivateList	*aL);
short PLGetMaxXValue( PrivateList	*aL);
Boolean PLConvertPoint( Point *cPt, PrivateList	*aL);
Boolean PLGetSelect( Point *myPt, PrivateList	*aL);
void PLGetSelectRect( Rect *dstRect, PrivateList	*aL);
void PLSetSelect( short left, short top, short right, short bottom, PrivateList	*aL);
void PLAutoScroll( PrivateList	*aL);
void PLSetControl( PrivateList		*aL);
Boolean PLClick( Point pt, short modifiers, PrivateList *aL);
void PLUpdate( PrivateList *aL);
pascal void PLactionProcPartition(ControlHandle theControl, short ctlPart);
void PLScroll( Point myPt, PrivateList *aL);
void PLDoArrows( short theChar, PrivateList	*aL);
void PLUpdateItem( Point cell, PrivateList *aL);
void PLRect( Rect *dstRect, Point cell, PrivateList	*aL);
void PLCheckSelect( PrivateList	*aL);
void PLSetMyIntList( PrivateList *aL);

#endif
